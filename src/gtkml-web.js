gtk_ml_web_init = undefined
gtk_ml_web_deinit = undefined
gtk_ml_web_version = undefined
gtk_ml_web_eval = undefined

gtk_web_ctx = undefined
gtk_web_builder = undefined
gtk_web_prev = undefined
gtk_web_nprev = undefined
gtk_web_history = []

function gtk_ml_js_read_stdin() {
    let result = document.getElementById('gtkml-stdin').value;
    document.getElementById('gtkml-stdin').value = '';
    return result;
}

function gtk_ml_js_init(_ctx, _builder, _prev, _nprev) {
    gtk_ml_web_init = Module.cwrap('gtk_ml_web_init', 'number', []); // number acts as GtkMl_Context *
    gtk_ml_web_deinit = Module.cwrap('gtk_ml_web_deinit', null, ['number', 'number', 'number', 'number']); // numbers act as GtkMl_Context *, GtkMl_Program ** and size_t *
    gtk_ml_web_version = Module.cwrap('gtk_ml_web_version', 'string', []);
    gtk_ml_web_eval = Module.cwrap('gtk_ml_web_eval', 'string', ['number', 'string', 'number', 'number', 'number']); // numbers act as GtkMl_Context *, GtkMl_Program ** and size_t *
    document.getElementById('gtkml-stdout').value = gtk_ml_web_version() + '\n';
    document.getElementById('gtkml-stderr').value = '';

    gtk_web_ctx = _ctx;
    gtk_web_builder = _builder;
    gtk_web_prev = _prev;
    gtk_web_nprev = _nprev;
}

function gtk_ml_js_run() {
    if (typeof(gtk_web_ctx) == 'undefined') {
        document.getElementById('gtkml-output').value = "Please wait until the web runtime has been initialized.";
        return;
    }
    gtk_web_history.push(document.getElementById('gtkml-input').value);
    let output = gtk_ml_web_eval(gtk_web_ctx, gtk_web_history[gtk_web_history.length - 1], gtk_web_builder, gtk_web_prev, gtk_web_nprev);
    document.getElementById('gtkml-output').value = output;
}
