#include <stdlib.h>
#include <string.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#define GTKML_A_BITS 5
#define GTKML_A_SIZE (1 << GTKML_A_BITS)
#define GTKML_A_MASK (GTKML_A_SIZE - 1)

typedef enum GtkMl_ArrayNodeKind {
    GTKML_A_LEAF,
    GTKML_A_BRANCH,
} GtkMl_ArrayNodeKind;

typedef struct GtkMl_ALeaf {
    GtkMl_S *value;
} GtkMl_ALeaf;

typedef struct GtkMl_ABranch {
    GtkMl_ArrayNode **nodes;
    size_t len;
} GtkMl_ABranch;

typedef union GtkMl_AUnion {
    GtkMl_ALeaf a_leaf;
    GtkMl_ABranch a_branch;
} GtkMl_AUnion;

struct GtkMl_ArrayNode {
    int rc;
    size_t shift; // (depth - 1) * GTKML_A_BITS
    GtkMl_ArrayNodeKind kind;
    GtkMl_AUnion value;
};

GTKML_PRIVATE GtkMl_ArrayNode *new_leaf(GtkMl_S *value);
GTKML_PRIVATE GtkMl_ArrayNode *new_branch(size_t shift, size_t len);
GTKML_PRIVATE GtkMl_ArrayNode *copy_node(GtkMl_ArrayNode *node);
GTKML_PRIVATE void del_node(GtkMl_Context *ctx, GtkMl_ArrayNode *node, void (*deleter)(GtkMl_Context *, GtkMl_S *));
GTKML_PRIVATE gboolean push(GtkMl_ArrayNode **out, GtkMl_ArrayNode *node, size_t *depth, GtkMl_S *value);
GTKML_PRIVATE GtkMl_S *get(GtkMl_ArrayNode *node, size_t index);
GTKML_PRIVATE GtkMl_S *delete(GtkMl_ArrayNode **out, gboolean *shiftme, GtkMl_ArrayNode *node, size_t index);
GTKML_PRIVATE GtkMl_VisitResult foreach(GtkMl_Array *array, GtkMl_ArrayNode *node, size_t index, GtkMl_ArrayFn fn, void *data);
GTKML_PRIVATE GtkMl_VisitResult foreach_rev(GtkMl_Array *array, GtkMl_ArrayNode *node, size_t index, GtkMl_ArrayFn fn, void *data);
GTKML_PRIVATE gboolean equal(GtkMl_ArrayNode *lhs, GtkMl_ArrayNode *rhs);
GTKML_PRIVATE GtkMl_VisitResult fn_contains(GtkMl_Array *array, size_t index, GtkMl_S *value, void *data);

void gtk_ml_new_array_trie(GtkMl_Array *array) {
    array->root = NULL;
    array->len = 0;
}

void gtk_ml_del_array_trie(GtkMl_Context *ctx, GtkMl_Array *array, void (*deleter)(GtkMl_Context *, GtkMl_S *)) {
    del_node(ctx, array->root, deleter);
}

void gtk_ml_array_trie_copy(GtkMl_Array *out, GtkMl_Array *array) {
    out->root = copy_node(array->root);
    out->len = array->len;
}

GTKML_PRIVATE GtkMl_VisitResult is_string(GtkMl_Array *ht, size_t index, GtkMl_S *value, void *data) {
    (void) ht;
    (void) index;

    gboolean *is_string = data;

    if (value->kind != GTKML_S_CHAR) {
        *is_string = 0;
        return GTKML_VISIT_BREAK;
    }

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_array_trie_is_string(GtkMl_Array *array) {
    gboolean is = 1;
    gtk_ml_array_trie_foreach(array, is_string, &is);
    return is;
}

size_t gtk_ml_array_trie_len(GtkMl_Array *array) {
    return array->len;
}

GTKML_PRIVATE GtkMl_VisitResult fn_concat(GtkMl_Array *ht, size_t index, GtkMl_S *value, void *data) {
    (void) ht;
    (void) index;

    GtkMl_Array *dest = data;

    GtkMl_Array new;
    gtk_ml_array_trie_push(&new, dest, value);
    *dest = new;

    return GTKML_VISIT_RECURSE;
}

void gtk_ml_array_trie_concat(GtkMl_Array *out, GtkMl_Array *lhs, GtkMl_Array *rhs) {
    out->root = copy_node(lhs->root);
    out->len = lhs->len;

    gtk_ml_array_trie_foreach(rhs, fn_concat, out);
}

void gtk_ml_array_trie_push(GtkMl_Array *out, GtkMl_Array *array, GtkMl_S *value) {
    out->root = NULL;
    out->len = array->len + 1;

    size_t depth;
    gboolean result = push(&out->root, array->root, &depth, value);
    if (!result) {
        out->root = new_branch(array->root->shift + GTKML_A_BITS, 2);
        out->root->value.a_branch.nodes[0] = copy_node(array->root);
        size_t shift = out->root->shift;
        GtkMl_ArrayNode **out_node = out->root->value.a_branch.nodes + 1;
        while (depth--) {
            shift -= GTKML_A_BITS;
            *out_node = new_branch(shift, 1);
            out_node = (*out_node)->value.a_branch.nodes;
        }
        *out_node = new_leaf(value);
    }
}

GtkMl_S *gtk_ml_array_trie_pop(GtkMl_Array *out, GtkMl_Array *array) {
    return gtk_ml_array_trie_delete(out, array, array->len - 1);
}

GtkMl_S *gtk_ml_array_trie_get(GtkMl_Array *array, size_t index) {
    return get(array->root, index);
}

struct ContainsData {
    GtkMl_S *value;
    gboolean contains;
    size_t index;
};

gboolean gtk_ml_array_trie_contains(GtkMl_Array *array, size_t *index, GtkMl_S *value) {
    struct ContainsData contains = { value, 0, 0 };
    foreach(array, array->root, 0, fn_contains, &contains);
    if (contains.contains) {
        *index = contains.index;
    }
    return contains.contains;
}

GtkMl_S *gtk_ml_array_trie_delete(GtkMl_Array *out, GtkMl_Array *array, size_t index) {
    out->root = NULL;
    out->len = array->len - 1;

    gboolean shiftme = 0;
    GtkMl_S *result = delete(&out->root, &shiftme, array->root, index);
    if (array->root->kind != GTKML_A_LEAF && shiftme) {
        size_t len = array->root->value.a_branch.len;
        size_t idx = index;
        memmove(&out->root->value.a_branch.nodes[idx], &out->root->value.a_branch.nodes[idx + 1], sizeof(GtkMl_ArrayNode *) * (len - idx - 1));
    }
    return result;
}

void gtk_ml_array_trie_foreach(GtkMl_Array *array, GtkMl_ArrayFn fn, void *data) {
    foreach(array, array->root, 0, fn, data);
}

void gtk_ml_array_trie_foreach_rev(GtkMl_Array *array, GtkMl_ArrayFn fn, void *data) {
    foreach_rev(array, array->root, 0, fn, data);
}

gboolean gtk_ml_array_trie_equal(GtkMl_Array *lhs, GtkMl_Array *rhs) {
    if (lhs->len != rhs->len) {
        return 0;
    }

    return equal(lhs->root, rhs->root);
}

GtkMl_ArrayNode *new_leaf(GtkMl_S *value) {
    GtkMl_ArrayNode *node = malloc(sizeof(GtkMl_ArrayNode));
    node->rc = 1;
    node->shift = -GTKML_A_BITS;
    node->kind = GTKML_A_LEAF;
    node->value.a_leaf.value = value;
    return node;
}

GtkMl_ArrayNode *new_branch(size_t shift, size_t len) {
    GtkMl_ArrayNode *node = malloc(sizeof(GtkMl_ArrayNode));
    node->rc = 1;
    node->shift = shift;
    node->kind = GTKML_A_BRANCH;
    node->value.a_branch.nodes = malloc(sizeof(GtkMl_ArrayNode *) * GTKML_A_SIZE);
    memset(node->value.a_branch.nodes, 0, sizeof(GtkMl_ArrayNode *) * GTKML_A_SIZE);
    node->value.a_branch.len = len;
    return node;
}

GtkMl_ArrayNode *copy_node(GtkMl_ArrayNode *node) {
    if (!node) {
        return NULL;
    }

    ++node->rc;

    return node;
}

void del_node(GtkMl_Context *ctx, GtkMl_ArrayNode *node, void (*deleter)(GtkMl_Context *, GtkMl_S *)) {
    if (!node) {
        return;
    }

    --node->rc;
    if (!node->rc) {
        switch (node->kind) {
        case GTKML_A_LEAF:
            deleter(ctx, node->value.a_leaf.value);
            break;
        case GTKML_A_BRANCH:
            for (size_t i = 0; i < node->value.a_branch.len; i++) {
                del_node(ctx, node->value.a_branch.nodes[i], deleter);
            }
            free(node->value.a_branch.nodes);
            break;
        }
        free(node);
    }
}

gboolean push(GtkMl_ArrayNode **out, GtkMl_ArrayNode *node, size_t *depth, GtkMl_S *value) {
    if (!node) {
        *out = new_leaf(value);
        return 1;
    }

    switch (node->kind) {
    case GTKML_A_LEAF:
        *depth = 0;
        return 0;
    case GTKML_A_BRANCH: {
        size_t len = node->value.a_branch.len;
        if (len == GTKML_A_SIZE) {
            *out = new_branch(node->shift, len);
        } else {
            *out = new_branch(node->shift, len + 1);
        }
        for (size_t i = 0; i < len; i++) {
            (*out)->value.a_branch.nodes[i] = copy_node(node->value.a_branch.nodes[i]);
        }
        size_t idx = len;
        if (idx == GTKML_A_SIZE) {
            gboolean result = push(&(*out)->value.a_branch.nodes[GTKML_A_SIZE - 1], node->value.a_branch.nodes[GTKML_A_SIZE - 1], depth, value);
            if (!result) {
                ++*depth;
            }
            return result;
        } else {
            gboolean result = push(&(*out)->value.a_branch.nodes[idx], node->value.a_branch.nodes[idx], depth, value);
            if (!result) {
                ++idx;
                size_t shift = (*out)->shift;
                GtkMl_ArrayNode **out_node = (*out)->value.a_branch.nodes + idx;
                while ((*depth)--) {
                    shift -= GTKML_A_BITS;
                    *out_node = new_branch(shift, 1);
                    out_node = (*out_node)->value.a_branch.nodes;
                }
                *out_node = new_leaf(value);
            }
            return result;
        }
    }
    }
}

GtkMl_S *get(GtkMl_ArrayNode *node, size_t index) {
    if (!node) {
        return NULL;
    }

    switch (node->kind) {
    case GTKML_A_LEAF:
        return node->value.a_leaf.value;
    case GTKML_A_BRANCH: {
        size_t idx = (index >> node->shift) & GTKML_A_MASK;
        return get(node->value.a_branch.nodes[idx], index);
    }
    }
}

GtkMl_S *delete(GtkMl_ArrayNode **out, gboolean *shiftme, GtkMl_ArrayNode *node, size_t index) {
    if (!node) {
        return NULL;
    }

    switch (node->kind) {
    case GTKML_A_LEAF:
        *shiftme = 1;
        del_node(NULL, *out, gtk_ml_delete_value_reference);
        *out = NULL;
        return node->value.a_leaf.value;
    case GTKML_A_BRANCH: {
        *out = new_branch(node->shift, node->value.a_branch.len);
        for (size_t i = 0; i < node->value.a_branch.len; i++) {
            (*out)->value.a_branch.nodes[i] = copy_node(node->value.a_branch.nodes[i]);
        }
        uint32_t idx = (index >> node->shift) & GTKML_A_MASK;
        gboolean shiftme = 0;
        GtkMl_S *result = delete(&(*out)->value.a_branch.nodes[idx], &shiftme, node->value.a_branch.nodes[idx], index);
        if (shiftme) {
            --(*out)->value.a_branch.len;
            size_t len = (*out)->value.a_branch.len;
            for (size_t i = idx; i < len; i++) {
                (*out)->value.a_branch.nodes[i] = node->value.a_branch.nodes[i + 1];
            }
        }
        return result;
    }
    }
}

GtkMl_VisitResult foreach(GtkMl_Array *array, GtkMl_ArrayNode *node, size_t index, GtkMl_ArrayFn fn, void *data) {
    if (!node) {
        return GTKML_VISIT_RECURSE;
    }

    switch (node->kind) {
    case GTKML_A_LEAF:
        return fn(array, index, node->value.a_leaf.value, data);
    case GTKML_A_BRANCH: {
        for (size_t i = 0; i < node->value.a_branch.len; i++) {
            switch (foreach(array, node->value.a_branch.nodes[i], (index << node->shift) | i, fn, data)) {
            case GTKML_VISIT_RECURSE:
                continue;
            case GTKML_VISIT_CONTINUE:
                return GTKML_VISIT_RECURSE;
            case GTKML_VISIT_BREAK:
                return GTKML_VISIT_BREAK;
            }
        }
        return GTKML_VISIT_RECURSE;
    }
    }
}

GtkMl_VisitResult foreach_rev(GtkMl_Array *array, GtkMl_ArrayNode *node, size_t index, GtkMl_ArrayFn fn, void *data) {
    if (!node) {
        return GTKML_VISIT_RECURSE;
    }

    switch (node->kind) {
    case GTKML_A_LEAF:
        return fn(array, index, node->value.a_leaf.value, data);
    case GTKML_A_BRANCH: {
        for (size_t i = node->value.a_branch.len; i > 0; i--) {
            switch (foreach_rev(array, node->value.a_branch.nodes[i - 1], (index << GTKML_A_BITS) | (i - 1), fn, data)) {
            case GTKML_VISIT_RECURSE:
                continue;
            case GTKML_VISIT_CONTINUE:
                return GTKML_VISIT_RECURSE;
            case GTKML_VISIT_BREAK:
                return GTKML_VISIT_BREAK;
            }
        }
        return GTKML_VISIT_RECURSE;
    }
    }
}

GtkMl_VisitResult fn_contains(GtkMl_Array *array, size_t index, GtkMl_S *value, void *data) {
    (void) array;

    struct ContainsData *contains = data;
    if (gtk_ml_equal(value, contains->value)) {
        contains->contains = 1;
        contains->index = index;
        return GTKML_VISIT_BREAK;
    } else {
        return GTKML_VISIT_RECURSE;
    }
}

gboolean equal(GtkMl_ArrayNode *lhs, GtkMl_ArrayNode *rhs) {
    if (lhs == rhs) {
        return 1;
    }

    if (lhs->kind != rhs->kind) {
        return 0;
    }

    switch (lhs->kind) {
    case GTKML_A_LEAF:
        return gtk_ml_equal(lhs->value.a_leaf.value, rhs->value.a_leaf.value);
    case GTKML_A_BRANCH:
        if (lhs->value.a_branch.len != rhs->value.a_branch.len) {
            return 0;
        }
        for (size_t i = 0; i < lhs->value.a_branch.len; i++) {
            if (!equal(lhs->value.a_branch.nodes[i], rhs->value.a_branch.nodes[i])) {
                return 0;
            }
        }
        return 1;
    }
}

#ifdef GTKML_ENABLE_POSIX
/* debug stuff */

// GTKML_PRIVATE GtkMl_ArrayNode *new_leaf_debug(GtkMl_Context *ctx, GtkMl_S *value);
// GTKML_PRIVATE GtkMl_ArrayNode *new_branch_debug(GtkMl_Context *ctx, size_t shift, size_t len);
GTKML_PRIVATE GtkMl_ArrayNode *copy_node_debug(GtkMl_Context *ctx, GtkMl_ArrayNode *node);
// GTKML_PRIVATE void del_node_debug(GtkMl_Context *ctx, GtkMl_ArrayNode *node, void (*deleter)(GtkMl_Context *, GtkMl_S *));
// GTKML_PRIVATE gboolean push_debug(GtkMl_Context *ctx, GtkMl_ArrayNode **out, GtkMl_ArrayNode *node, size_t *depth, GtkMl_S *value);
GTKML_PRIVATE GtkMl_S *get_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_ArrayNode *node, size_t index);
// GTKML_PRIVATE GtkMl_S *delete_debug(GtkMl_Context *ctx, GtkMl_ArrayNode **out, gboolean *shiftme, GtkMl_ArrayNode *node, size_t index);
GTKML_PRIVATE GtkMl_VisitResult foreach_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, GtkMl_ArrayNode *node, size_t index, GtkMl_ArrayDebugFn fn, void *data);
GTKML_PRIVATE gboolean equal_debug(GtkMl_Context *ctx, GtkMl_ArrayNode *lhs, GtkMl_ArrayNode *rhs);
GTKML_PRIVATE GtkMl_VisitResult fn_contains_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t index, GtkMl_S *value, void *data);

void gtk_ml_array_trie_copy_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array) {
    out->root = copy_node_debug(ctx, array->root);
    out->len = array->len;
}

GTKML_PRIVATE GtkMl_VisitResult is_string_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *ht, size_t index, GtkMl_S *value, void *data) {
    (void) ht;
    (void) index;

    gboolean *is_string = data;

    GtkMl_S val;
    gtk_ml_dbg_read_value(ctx, err, &val, value);

    if (val.kind != GTKML_S_CHAR) {
        *is_string = 0;
        return GTKML_VISIT_BREAK;
    }

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_array_trie_is_string_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array) {
    gboolean is = 1;
    gtk_ml_array_trie_foreach_debug(ctx, err, array, is_string_debug, &is);
    return is;
}

size_t gtk_ml_array_trie_len_debug(GtkMl_Context *ctx, GtkMl_Array *array) {
    (void) ctx;
    return array->len;
}

GTKML_PRIVATE GtkMl_VisitResult fn_concat_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *ht, size_t index, GtkMl_S *value, void *data) {
    (void) ctx;
    (void) err;
    (void) ht;
    (void) index;

    GtkMl_Array *dest = data;

    GtkMl_Array new;
    gtk_ml_array_trie_push_debug(ctx, &new, dest, value);
    *dest = new;

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_array_trie_concat_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *out, GtkMl_Array *lhs, GtkMl_Array *rhs) {
    out->root = copy_node(lhs->root);
    out->len = lhs->len;

    return gtk_ml_array_trie_foreach_debug(ctx, err, rhs, fn_concat_debug, out);
}

void gtk_ml_array_trie_push_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array, GtkMl_S *value) {
    (void) ctx;
    (void) out;
    (void) array;
    (void) value;
    fprintf(stderr, "warning: push is currently unavailable in debug mode\n");
}

GtkMl_S *gtk_ml_array_trie_pop_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array) {
    return gtk_ml_array_trie_delete_debug(ctx, out, array, array->len - 1);
}

GtkMl_S *gtk_ml_array_trie_get_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t index) {
    return get_debug(ctx, err, array->root, index);
}

struct ContainsDebugData {
    GtkMl_S *value;
    gboolean contains;
    size_t index;
};

gboolean gtk_ml_array_trie_contains_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t *index, GtkMl_S *value) {
    struct ContainsDebugData contains = { value, 0, 0 };
    foreach_debug(ctx, err, array, array->root, 0, fn_contains_debug, &contains);
    if (contains.contains) {
        *index = contains.index;
    }
    return contains.contains;
}

GtkMl_S *gtk_ml_array_trie_delete_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array, size_t index) {
    (void) ctx;
    (void) out;
    (void) array;
    (void) index;
    fprintf(stderr, "warning: delete is currently unavailable in debug mode\n");
    return NULL;
}

gboolean gtk_ml_array_trie_foreach_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, GtkMl_ArrayDebugFn fn, void *data) {
    *err = NULL;
    foreach_debug(ctx, err, array, array->root, 0, fn, data);
    return *err == NULL;
}

gboolean gtk_ml_array_trie_equal_debug(GtkMl_Context *ctx, GtkMl_Array *lhs, GtkMl_Array *rhs) {
    if (lhs->len != rhs->len) {
        return 0;
    }

    return equal_debug(ctx, lhs->root, rhs->root);
}

// GtkMl_ArrayNode *new_leaf_debug(GtkMl_Context *ctx, GtkMl_S *value) {
//     (void) ctx;
//     (void) value;
//     fprintf(stderr, "warning: new_leaf is currently unavailable in debug mode\n");
//     return NULL;
// }
// 
// GtkMl_ArrayNode *new_branch_debug(GtkMl_Context *ctx, size_t shift, size_t len) {
//     (void) ctx;
//     (void) shift;
//     (void) len;
//     fprintf(stderr, "warning: new_branch is currently unavailable in debug mode\n");
//     return NULL;
// }

GtkMl_ArrayNode *copy_node_debug(GtkMl_Context *ctx, GtkMl_ArrayNode *node) {
    (void) ctx;
    (void) node;
    fprintf(stderr, "warning: copy_node is currently unavailable in debug mode\n");
    return NULL;
}

// void del_node_debug(GtkMl_Context *ctx, GtkMl_ArrayNode *node, void (*deleter)(GtkMl_Context *, GtkMl_S *)) {
//     (void) ctx;
//     (void) node;
//     (void) deleter;
//     fprintf(stderr, "warning: del_node is currently unavailable in debug mode\n");
// }
// 
// gboolean push_debug(GtkMl_Context *ctx, GtkMl_ArrayNode **out, GtkMl_ArrayNode *node, size_t *depth, GtkMl_S *value) {
//     (void) ctx;
//     (void) out;
//     (void) node;
//     (void) depth;
//     (void) value;
//     fprintf(stderr, "warning: push is currently unavailable in debug mode\n");
//     return 0;
// }

GtkMl_S *get_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_ArrayNode *node, size_t index) {
    if (!node) {
        return NULL;
    }

    GtkMl_ArrayNodeKind kind = (GtkMl_ArrayNodeKind) gtk_ml_dbg_read_u32(ctx, err, &node->kind);
    if (*err) {
        return NULL;
    }
    switch (kind) {
    case GTKML_A_LEAF:
        return gtk_ml_dbg_read_ptr(ctx, err, &node->value.a_leaf.value);
    case GTKML_A_BRANCH: {
        size_t shift = gtk_ml_dbg_read_u64(ctx, err, &node->shift);
        size_t idx = (index >> shift) & GTKML_A_MASK;
        GtkMl_ArrayNode *next = gtk_ml_dbg_read_ptr(ctx, err, &node->value.a_branch.nodes[idx]);
        if (*err) {
            return NULL;
        }
        return get_debug(ctx, err, next, index);
    }
    }
}

// GtkMl_S *delete_debug(GtkMl_Context *ctx, GtkMl_ArrayNode **out, gboolean *shiftme, GtkMl_ArrayNode *node, size_t index) {
//     (void) ctx;
//     (void) out;
//     (void) shiftme;
//     (void) node;
//     (void) index;
//     fprintf(stderr, "warning: delete is currently unavailable in debug mode\n");
//     return NULL;
// }

GtkMl_VisitResult foreach_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, GtkMl_ArrayNode *node, size_t index, GtkMl_ArrayDebugFn fn, void *data) {
    if (!node) {
        return GTKML_VISIT_RECURSE;
    }

    GtkMl_ArrayNodeKind kind = (GtkMl_ArrayNodeKind) gtk_ml_dbg_read_u32(ctx, err, &node->kind);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    switch (kind) {
    case GTKML_A_LEAF: {
        GtkMl_S *value = gtk_ml_dbg_read_ptr(ctx, err, &node->value.a_leaf.value);
        if (*err) {
            return GTKML_VISIT_BREAK;
        }
        return fn(ctx, err, array, index, value, data);
    }
    case GTKML_A_BRANCH: {
        size_t len = gtk_ml_dbg_read_u64(ctx, err, &node->value.a_branch.len);
        if (*err) {
            return GTKML_VISIT_BREAK;
        }
        size_t shift = gtk_ml_dbg_read_u64(ctx, err, &node->shift);
        if (*err) {
            return GTKML_VISIT_BREAK;
        }
        for (size_t i = 0; i < len; i++) {
            GtkMl_ArrayNode **nodes = gtk_ml_dbg_read_ptr(ctx, err, &node->value.a_branch.nodes);
            if (*err) {
                return GTKML_VISIT_BREAK;
            }
            GtkMl_ArrayNode *next = gtk_ml_dbg_read_ptr(ctx, err, &nodes[i]);
            if (*err) {
                return GTKML_VISIT_BREAK;
            }
            switch (foreach_debug(ctx, err, array, next, (index << shift) | i, fn, data)) {
            case GTKML_VISIT_RECURSE:
                continue;
            case GTKML_VISIT_CONTINUE:
                return GTKML_VISIT_RECURSE;
            case GTKML_VISIT_BREAK:
                return GTKML_VISIT_BREAK;
            }
        }
        return GTKML_VISIT_RECURSE;
    }
    }
}

GtkMl_VisitResult fn_contains_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t index, GtkMl_S *value, void *data) {
    (void) ctx;
    (void) err;
    (void) array;
    (void) index;
    (void) value;
    (void) data;
    fprintf(stderr, "warning: contains is currently unavailable in debug mode\n");
    return GTKML_VISIT_BREAK;
}

gboolean equal_debug(GtkMl_Context *ctx, GtkMl_ArrayNode *lhs, GtkMl_ArrayNode *rhs) {
    if (lhs == rhs) {
        return 1;
    }

    (void) ctx;
    fprintf(stderr, "warning: equal is currently unavailable in debug mode\n");
    return 0;
}
#endif /* GTKML_ENABLE_POSIX */
