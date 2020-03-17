let gtk_web_canvas = document.getElementById("gtkml-canvas");
gtk_web_canvas.clientWidth = 400;
gtk_web_canvas.clientHeight = 300;
gtk_web_canvas.width = 400;
gtk_web_canvas.height = 300;
gtk_web_gl_objects = []
gtk_web_gl = gtk_web_canvas.getContext("webgl2");
if (!gtk_web_gl) {
    Module.printErr("warning: couldn't acquire webgl2 context\n");
}
