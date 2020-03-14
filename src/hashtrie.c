#include <stdio.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#define GTKML_H_BITS 5
#define GTKML_H_SIZE (1 << GTKML_H_BITS)
#define GTKML_H_MASK (GTKML_H_SIZE - 1)

typedef enum GtkMl_HashTrieNodeKind {
    GTKML_HT_LEAF,
    GTKML_HT_BRANCH,
} GtkMl_HashTrieNodeKind;

typedef struct GtkMl_HLeaf {
    void *key;
    void *value;
} GtkMl_HLeaf;

typedef struct GtkMl_HBranch {
    GtkMl_HashTrieNode **nodes;
} GtkMl_HBranch;

typedef union GtkMl_HUnion {
    GtkMl_HLeaf h_leaf;
    GtkMl_HBranch h_branch;
} GtkMl_HUnion;

struct GtkMl_HashTrieNode {
    int rc;
    GtkMl_HashTrieNodeKind kind;
    GtkMl_HUnion value;
};

GTKML_PRIVATE GtkMl_HashTrieNode *new_leaf(void *key, void *value);
GTKML_PRIVATE GtkMl_HashTrieNode *new_branch();
GTKML_PRIVATE GtkMl_HashTrieNode *copy_node(GtkMl_HashTrieNode *node);
GTKML_PRIVATE void del_node(GtkMl_Context *ctx, GtkMl_HashTrieNode *node, void (*deleter)(GtkMl_Context *, void *));
GTKML_PRIVATE void *insert(GtkMl_Hasher *hasher, GtkMl_HashTrieNode **out, size_t *inc, GtkMl_HashTrieNode *node, void *key, void *value, GtkMl_Hash hash, uint32_t shift);
GTKML_PRIVATE void *get(GtkMl_Hasher *hasher, GtkMl_HashTrieNode *node, void *key, GtkMl_Hash hash, uint32_t shift);
GTKML_PRIVATE void *delete(GtkMl_Hasher *hasher, GtkMl_HashTrieNode **out, size_t *dec, GtkMl_HashTrieNode *node, void *key, GtkMl_Hash hash, uint32_t shift);
GTKML_PRIVATE GtkMl_VisitResult foreach(GtkMl_HashTrie *ht, GtkMl_HashTrieNode *node, GtkMl_HashTrieFn fn, void *data);
GTKML_PRIVATE gboolean equal(GtkMl_Hasher *hasher, GtkMl_HashTrieNode *lhs, GtkMl_HashTrieNode *rhs);

void gtk_ml_new_hash_trie(GtkMl_HashTrie *ht, GtkMl_Hasher *hasher) {
    ht->hasher = hasher;
    ht->root = NULL;
    ht->len = 0;
}

void gtk_ml_del_hash_trie(GtkMl_Context *ctx, GtkMl_HashTrie *ht, void (*deleter)(GtkMl_Context *, void *)) {
    del_node(ctx, ht->root, deleter);
}

void gtk_ml_hash_trie_copy(GtkMl_HashTrie *out, GtkMl_HashTrie *ht) {
    out->hasher = ht->hasher;
    out->root = copy_node(ht->root);
    out->len = ht->len;
}

size_t gtk_ml_hash_trie_len(GtkMl_HashTrie *ht) {
    return ht->len;
}

GTKML_PRIVATE GtkMl_VisitResult fn_concat(GtkMl_HashTrie *ht, void *key, void *value, void *data) {
    (void) ht;

    GtkMl_HashTrie *dest = data;

    GtkMl_HashTrie new;
    gtk_ml_hash_trie_insert(&new, dest, key, value);
    *dest = new;
    
    return GTKML_VISIT_RECURSE;
}

void gtk_ml_hash_trie_concat(GtkMl_HashTrie *out, GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs) {
    out->hasher = lhs->hasher;
    out->root = copy_node(lhs->root);
    out->len = lhs->len;

    gtk_ml_hash_trie_foreach(rhs, fn_concat, out);
}

void *gtk_ml_hash_trie_insert(GtkMl_HashTrie *out, GtkMl_HashTrie *ht, void *key, void *value) {
    out->hasher = ht->hasher;
    out->root = NULL;
    out->len = ht->len;

    GtkMl_Hash hash;
    if (!gtk_ml_hash(ht->hasher, &hash, key)) {
        return NULL;
    }
    return insert(ht->hasher, &out->root, &out->len, ht->root, key, value, hash, 0);
}

void *gtk_ml_hash_trie_get(GtkMl_HashTrie *ht, void *key) {
    GtkMl_Hash hash;
    if (!gtk_ml_hash(ht->hasher, &hash, key)) {
        return NULL;
    }
    return get(ht->hasher, ht->root, key, hash, 0);
}

gboolean gtk_ml_hash_trie_contains(GtkMl_HashTrie *ht, void *key) {
    GtkMl_Hash hash;
    if (!gtk_ml_hash(ht->hasher, &hash, key)) {
        return 0;
    }
    return get(ht->hasher, ht->root, key, hash, 0) != NULL;
}

void *gtk_ml_hash_trie_delete(GtkMl_HashTrie *out, GtkMl_HashTrie *ht, void *key) {
    out->hasher = ht->hasher;
    out->root = NULL;
    out->len = ht->len;

    GtkMl_Hash hash;
    if (!gtk_ml_hash(ht->hasher, &hash, key)) {
        return NULL;
    }
    return delete(out->hasher, &out->root, &out->len, ht->root, key, hash, 0);
}

void gtk_ml_hash_trie_foreach(GtkMl_HashTrie *ht, GtkMl_HashTrieFn fn, void *data) {
    foreach(ht, ht->root, fn, data);
}

gboolean gtk_ml_hash_trie_equal(GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs) {
    if (lhs->len != rhs->len) {
        return 0;
    }

    return equal(lhs->hasher, lhs->root, rhs->root);
}

GtkMl_HashTrieNode *new_leaf(void *key, void *value) {
    GtkMl_HashTrieNode *node = malloc(sizeof(GtkMl_HashTrieNode));
    node->rc = 1;
    node->kind = GTKML_HT_LEAF;
    node->value.h_leaf.key = key;
    node->value.h_leaf.value = value;
    return node;
}

GtkMl_HashTrieNode *new_branch() {
    GtkMl_HashTrieNode *node = malloc(sizeof(GtkMl_HashTrieNode));
    node->rc = 1;
    node->kind = GTKML_HT_BRANCH;
    node->value.h_branch.nodes = malloc(sizeof(GtkMl_HashTrieNode *) * GTKML_H_SIZE);
    memset(node->value.h_branch.nodes, 0, sizeof(GtkMl_HashTrieNode *) * GTKML_H_SIZE);
    return node;
}

GtkMl_HashTrieNode *copy_node(GtkMl_HashTrieNode *node) {
    if (!node) {
        return NULL;
    }

    ++node->rc;

    return node;
}

void del_node(GtkMl_Context *ctx, GtkMl_HashTrieNode *node, void (*deleter)(GtkMl_Context *, void *)) {
    if (!node) {
        return;
    }

    --node->rc;
    if (!node->rc) {
        switch (node->kind) {
        case GTKML_HT_LEAF:
            deleter(ctx, node->value.h_leaf.key);
            deleter(ctx, node->value.h_leaf.value);
            break;
        case GTKML_HT_BRANCH:
            for (size_t i = 0; i < GTKML_H_SIZE; i++) {
                del_node(ctx, node->value.h_branch.nodes[i], deleter);
            }
            free(node->value.h_branch.nodes);
            break;
        }
        free(node);
    }
}

void *insert(GtkMl_Hasher *hasher, GtkMl_HashTrieNode **out, size_t *inc, GtkMl_HashTrieNode *node, void *key, void *value, GtkMl_Hash hash, uint32_t shift) {
    if (!node) {
        ++*inc;
        *out = new_leaf(key, value);
        return NULL;
    }

    switch (node->kind) {
    case GTKML_HT_LEAF:
        if (hasher->equal(key, node->value.h_leaf.key)) {
            *out = new_leaf(key, value);
            return node->value.h_leaf.value;
        } else {
            *out = new_branch();

            GtkMl_Hash _hash;
            gtk_ml_hash(hasher, &_hash, node->value.h_leaf.key);
            uint32_t _idx = (_hash >> shift) & GTKML_H_MASK;
            (*out)->value.h_branch.nodes[_idx] = copy_node(node);

            if (hash == _hash) {
                fprintf(stderr, "fatal error: two non-equal keys in a hash map have the same hashes at %p, %p\n  ", node->value.h_leaf.key, key);
                gtk_ml_dumpf(NULL, stderr, NULL, node->value.h_leaf.key);
                fprintf(stderr, ";");
                gtk_ml_dumpf(NULL, stderr, NULL, key);
                fprintf(stderr, "\n");
                exit(1);
            }

            uint32_t idx = (hash >> shift) & GTKML_H_MASK;
            return insert(hasher, &(*out)->value.h_branch.nodes[idx], inc, (*out)->value.h_branch.nodes[idx], key, value, hash, shift + GTKML_H_BITS);
        }
    case GTKML_HT_BRANCH: {
        *out = new_branch();
        for (size_t i = 0; i < GTKML_H_SIZE; i++) {
            (*out)->value.h_branch.nodes[i] = copy_node(node->value.h_branch.nodes[i]);
        }
        uint32_t idx = (hash >> shift) & GTKML_H_MASK;
        return insert(hasher, &(*out)->value.h_branch.nodes[idx], inc, node->value.h_branch.nodes[idx], key, value, hash, shift + GTKML_H_BITS);
    }
    }
}

void *get(GtkMl_Hasher *hasher, GtkMl_HashTrieNode *node, void *key, GtkMl_Hash hash, uint32_t shift) {
    if (!node) {
        return NULL;
    }

    switch (node->kind) {
    case GTKML_HT_LEAF:
        if (hasher->equal(node->value.h_leaf.key, key)) {
            return node->value.h_leaf.value;
        } else {
            return NULL;
        }
    case GTKML_HT_BRANCH: {
        uint32_t idx = (hash >> shift) & GTKML_H_MASK;
        return get(hasher, node->value.h_branch.nodes[idx], key, hash, shift + GTKML_H_BITS);
    }
    }
}

void *delete(GtkMl_Hasher *hasher, GtkMl_HashTrieNode **out, size_t *dec, GtkMl_HashTrieNode *node, void *key, GtkMl_Hash hash, uint32_t shift) {
    if (!node) {
        return NULL;
    }

    switch (node->kind) {
    case GTKML_HT_LEAF:
        if (hasher->equal(node->value.h_leaf.key, key)) {
            --*dec;
            del_node(NULL, *out, gtk_ml_delete_void_reference);
            *out = NULL;
            return node->value.h_leaf.value;
        } else {
            return NULL;
        }
    case GTKML_HT_BRANCH: {
        *out = new_branch();
        for (size_t i = 0; i < GTKML_H_SIZE; i++) {
            (*out)->value.h_branch.nodes[i] = copy_node(node->value.h_branch.nodes[i]);
        }
        uint32_t idx = (hash >> shift) & GTKML_H_MASK;
        return delete(hasher, &(*out)->value.h_branch.nodes[idx], dec, node->value.h_branch.nodes[idx], key, hash, shift + GTKML_H_BITS);
    }
    }
}

GtkMl_VisitResult foreach(GtkMl_HashTrie *ht, GtkMl_HashTrieNode *node, GtkMl_HashTrieFn fn, void *data) {
    if (!node) {
        return GTKML_VISIT_RECURSE;
    }

    switch (node->kind) {
    case GTKML_HT_LEAF:
        return fn(ht, node->value.h_leaf.key, node->value.h_leaf.value, data);
    case GTKML_HT_BRANCH: {
        for (size_t i = 0; i < GTKML_H_SIZE; i++) {
            switch (foreach(ht, node->value.h_branch.nodes[i], fn, data)) {
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

gboolean equal(GtkMl_Hasher *hasher, GtkMl_HashTrieNode *lhs, GtkMl_HashTrieNode *rhs) {
    if (lhs == rhs) {
        return 1;
    }

    if (lhs->kind != rhs->kind) {
        return 0;
    }

    switch (lhs->kind) {
    case GTKML_HT_LEAF:
        return hasher->equal(lhs->value.h_leaf.key, rhs->value.h_leaf.key) && hasher->equal(lhs->value.h_leaf.value, rhs->value.h_leaf.value);
    case GTKML_HT_BRANCH:
        for (size_t i = 0; i < GTKML_H_SIZE; i++) {
            if (!equal(hasher, lhs->value.h_branch.nodes[i], rhs->value.h_branch.nodes[i])) {
                return 0;
            }
        }
        return 1;
    }
}
