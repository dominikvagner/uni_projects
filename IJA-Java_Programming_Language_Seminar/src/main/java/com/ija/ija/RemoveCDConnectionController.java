package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import com.ija.ija.uml.UMLClass;
import com.ija.ija.uml.UMLClassConnection;
import com.ija.ija.uml.UMLOperation;
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

import java.util.concurrent.atomic.AtomicBoolean;

public class RemoveCDConnectionController {
    public static boolean display(ClassDiagram classDiagram, UMLClass umlClass) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Remove a class connection from this class.");

        // Inputs
        Label label = new Label("Choose the connection to remove:");
        label.setPadding(new Insets(10));
        ChoiceBox<UMLClassConnection> choiceBox = new ChoiceBox<>();
        for (UMLClassConnection conn : classDiagram.getConnections()) {
            if (conn.getClassFrom().equals(umlClass.getName()))
                choiceBox.getItems().add(conn);
        }
        choiceBox.setMaxWidth(Double.MAX_VALUE);

        // Button
        Button button = new Button("Remove");
        button.setOnAction(actionEvent -> {
            if (choiceBox.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please choose a connection to remove!");
                alertError.showAndWait();
            } else {
                UMLClassConnection chosenConn = choiceBox.getSelectionModel().getSelectedItem();
                classDiagram.getConnections().remove(chosenConn);
                if (chosenConn.getType().equals("Generalization")) {
                    for (UMLOperation op : classDiagram.findClass(chosenConn.getClassTo()).getOperations()) {
                        umlClass.getGeneralizedOperations().remove(op);
                    }
                    for (UMLOperation op : classDiagram.findClass(chosenConn.getClassTo()).getGeneralizedOperations()) {
                        umlClass.getGeneralizedOperations().remove(op);
                    }
                }
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
