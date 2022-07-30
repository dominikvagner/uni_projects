package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import com.ija.ija.uml.UMLClass;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.concurrent.atomic.AtomicBoolean;

public class EditCDClassController {
    public static boolean display(ClassDiagram classDiagram, UMLClass umlClass) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Edit class in class diagram.");

        // Inputs
        Label label = new Label("Rename class, new class name:");
        label.setPadding(new Insets(10));
        TextField textField = new TextField();

        // Buttons
        Button buttonRename = new Button("Rename");
        buttonRename.setOnAction(actionEvent -> {
            if (textField.getText().length() == 0) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Can't rename class to empty name.");
                alertError.showAndWait();
            } else {
                classDiagram.renameClass(umlClass.getName(), textField.getText());
                result.set(true);
                stage.close();
            }
        });

        Button buttonDelete = new Button("Delete");
        buttonDelete.setStyle("-fx-text-fill: white; -fx-background-color: red");
        buttonDelete.setOnAction(actionEvent -> {
            Alert alert = new Alert(
                    Alert.AlertType.CONFIRMATION,
                    "Are you sure you want to delete this class?",
                    ButtonType.YES, ButtonType.NO
            );
            alert.showAndWait();

            if (alert.getResult() == ButtonType.YES) {
                classDiagram.removeClass(umlClass.getName());
                result.set(true);
                stage.close();
            }
        });

        // Layout
        HBox inputLayout1 = new HBox();
        inputLayout1.getChildren().addAll(label, textField);
        inputLayout1.setAlignment(Pos.CENTER_LEFT);
        inputLayout1.setPadding(new Insets(20));
        inputLayout1.setSpacing(20);

        HBox inputLayout2 = new HBox();
        inputLayout2.getChildren().addAll(buttonRename, buttonDelete);
        inputLayout2.setAlignment(Pos.CENTER);
        inputLayout2.setPadding(new Insets(20));
        inputLayout2.setSpacing(20);

        VBox layout = new VBox();
        layout.getChildren().addAll(inputLayout1, inputLayout2);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
