gtk_ml_web_init = undefined
gtk_ml_web_deinit = undefined
gtk_ml_web_version = undefined
gtk_ml_web_eval = undefined

gtk_ml_ctx = undefined

function gtk_ml_js_read_stdin() {
    let result = document.getElementById('gtkml-stdin').value;
    document.getElementById('gtkml-stdin').value = '';
    return result;
}

function gtk_ml_js_init(_ctx) {
    document.getElementById('gtkml-stdout').value = '';
    document.getElementById('gtkml-stderr').value = '';
    gtk_ml_web_init = Module.cwrap('gtk_ml_web_init', 'number', []); // number acts as GtkMl_Context *
    gtk_ml_web_deinit = Module.cwrap('gtk_ml_web_deinit', null, ['number']); // number acts as GtkMl_Context *
    gtk_ml_web_version = Module.cwrap('gtk_ml_web_version', 'string', []);
    gtk_ml_web_eval = Module.cwrap('gtk_ml_web_eval', 'string', ['number', 'string']); // number acts as GtkMl_Context *

    gtk_ml_ctx = _ctx;
}

function gtk_ml_js_run() {
    if (typeof(gtk_ml_ctx) == 'undefined') {
        document.getElementById('gtkml-output').value = "Please wait until the web runtime has been initialized.";
        return;
    }
    let output = gtk_ml_web_eval(gtk_ml_ctx, document.getElementById('gtkml-input').value);
    document.getElementById('gtkml-output').value = output;
}
