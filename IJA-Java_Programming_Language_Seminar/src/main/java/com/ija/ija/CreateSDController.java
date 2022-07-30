package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.concurrent.atomic.AtomicBoolean;

public class CreateSDController {
    public static boolean display(ClassDiagram classDiagram) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Create/Add sequence diagram.");
        Label label = new Label("Diagram name:");
        label.setPadding(new Insets(10));
        TextField textField = new TextField();
        Button button = new Button("Create");
        button.setOnAction(actionEvent -> {
            if (textField.getText().length() == 0) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Can't create sequence diagram with no name.");
                alertError.showAndWait();
            } else if (classDiagram.getSequenceDiagrams().stream().anyMatch(n -> n.getName().equals(textField.getText()))) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Sequence diagram with this name already exists, please choose a different one.");
                alertError.showAndWait();
            } else {
                classDiagram.createSeqDiagram(textField.getText());
                result.set(true);
                stage.close();
            }
        });

        HBox inputLayout = new HBox();
        inputLayout.getChildren().addAll(label, textField);
        inputLayout.setAlignment(Pos.CENTER);
        inputLayout.setPadding(new Insets(20));
        inputLayout.setSpacing(20);

        VBox layout = new VBox();
        layout.getChildren().addAll(inputLayout, button);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
