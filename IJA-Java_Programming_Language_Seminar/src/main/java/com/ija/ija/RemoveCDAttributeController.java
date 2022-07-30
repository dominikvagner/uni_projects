package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import com.ija.ija.uml.UMLAttribute;
import com.ija.ija.uml.UMLSequenceConnection;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.stream.Collectors;

public class RemoveCDAttributeController {
    public static boolean display(List<UMLAttribute> attributes) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Remove an attribute from this class.");

        // Inputs
        Label label = new Label("Choose the attribute to remove:");
        label.setPadding(new Insets(10));
        ChoiceBox<UMLAttribute> choiceBox = new ChoiceBox<>();
        for (UMLAttribute attr : attributes) {
            choiceBox.getItems().add(attr);
        }
        choiceBox.setMaxWidth(Double.MAX_VALUE);

        // Button
        Button button = new Button("Remove");
        button.setOnAction(actionEvent -> {
            if (choiceBox.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please choose an attribute to remove!");
                alertError.showAndWait();
            } else {
                attributes.remove(choiceBox.getSelectionModel().getSelectedItem());
                stage.close();
                result.set(true);
            }
        });

        // Layout
        HBox inputLayout1 = new HBox();
        inputLayout1.getChildren().addAll(label, choiceBox);
        inputLayout1.setAlignment(Pos.CENTER_LEFT);
        inputLayout1.setPadding(new Insets(20));
        inputLayout1.setSpacing(20);
        HBox.setHgrow(choiceBox, Priority.ALWAYS);

        VBox layout = new VBox();
        layout.getChildren().addAll(inputLayout1, button);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
