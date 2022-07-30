module com.ija.ija {
    requires javafx.graphics;
    requires javafx.controls;
    requires javafx.media;
    requires javafx.base;
    requires javafx.web;
    requires javafx.swing;
    requires javafx.fxml;
    requires com.fasterxml.jackson.databind;
    requires org.junit.jupiter.api;
    requires org.jetbrains.annotations;
    requires jfxtras.labs;

    opens com.ija.ija to javafx.fxml;
    exports com.ija.ija;
    exports com.ija.ija.uml;
    exports com.ija.ija.json;
}