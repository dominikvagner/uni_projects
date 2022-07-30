package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import com.ija.ija.uml.UMLAttribute;
import com.ija.ija.uml.UMLClass;
import com.ija.ija.uml.UMLOperation;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import javafx.stage.Stage;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

public class AddCDOperationController {
    public static boolean display(ClassDiagram classDiagram, UMLClass umlClass) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Add an operation to this class.");

        // Inputs
        Label labelMod = new Label("Choose operations access modifier:");
        labelMod.setPadding(new Insets(5, 10, 5, 10));
        ChoiceBox<String> choiceBoxMod = new ChoiceBox<>();
        for (String mod : classDiagram.getAccessMods()) {
            choiceBoxMod.getItems().add(mod);
        }
        choiceBoxMod.setMaxWidth(Double.MAX_VALUE);

        Label labelName = new Label("New operation name:");
        labelName.setPadding(new Insets(5, 10, 5, 10));
        TextField textFieldName = new TextField();

        Label labelType = new Label("Choose the return type of the operation:");
        labelType.setPadding(new Insets(5, 10, 5, 10));
        ChoiceBox<String> choiceBoxType = new ChoiceBox<>();
        for (String mod : classDiagram.printAllClassifiers()) {
            choiceBoxType.getItems().add(mod);
        }
        choiceBoxType.setMaxWidth(Double.MAX_VALUE);



        //Arguments
        VBox argumentsRight = new VBox();
        argumentsRight.setAlignment(Pos.TOP_LEFT);
        argumentsRight.setPadding(new Insets(10, 10, 5, 10));
        argumentsRight.setSpacing(5);
        argumentsRight.getChildren().add(new Text("No arguments added yet."));
        List<UMLAttribute> arguments = new ArrayList<>();

        Label labelArguments = new Label("Operation arguments:");
        labelType.setPadding(new Insets(5, 10, 5, 10));

        Label labelList = new Label("The operation return is a list of the selected type:");
        labelList.setPadding(new Insets(10));
        CheckBox checkBox = new CheckBox("Is list?");

        Button buttonAddArgument = new Button("Add argument");
        buttonAddArgument.setMaxWidth(Double.MAX_VALUE);
        buttonAddArgument.setOnAction(actionEvent -> {
            UMLAttribute newArg = AddCDAttributeController.display(classDiagram, umlClass, true);
            if (arguments.stream().anyMatch(a -> a.getName().equals(newArg.getName()))) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Attribute with this name already created, choose a different name!");
                alertError.showAndWait();
            } else {
                arguments.add(newArg);
                argumentsRight.getChildren().clear();
                for (UMLAttribute attr : arguments) {
                    argumentsRight.getChildren().add(new Text(attr.toString()));
                }
            }
        });

        Button buttonRemoveArgument = new Button("Remove Argument");
        buttonRemoveArgument.setOnAction(actionEvent -> {
            if (arguments.size() == 0) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please add some operation arguments before trying to remove some!");
                alertError.showAndWait();
            } else {
                if (RemoveCDAttributeController.display(arguments)) {
                    argumentsRight.getChildren().clear();
                    for (UMLAttribute attr : arguments) {
                        argumentsRight.getChildren().add(new Text(attr.toString()));
                    }
                }
            }
        });

        // Main button
        Button button = new Button("Add operation");
        button.setOnAction(actionEvent -> {
            if (choiceBoxMod.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("No access modifier chosen!");
                alertError.showAndWait();
            } else if (textFieldName.getText().length() == 0) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Can't create an operation with no name!");
                alertError.showAndWait();
            } else if (choiceBoxType.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("No operation return type chosen!");
                alertError.showAndWait();
            } else {
                UMLOperation newOp = new UMLOperation(
                        textFieldName.getText(),
                        classDiagram.classifierForName(choiceBoxType.getSelectionModel().getSelectedItem()),
                        choiceBoxMod.getSelectionModel().getSelectedItem(),
                        checkBox.isSelected()
                );
                if (umlClass.getOperations().stream().anyMatch(a -> a.getName().equals(newOp.getName()))) {
                    Alert alertError = new Alert(Alert.AlertType.ERROR);
                    alertError.setContentText("Attribute with this name already created, choose a different name!");
                    alertError.showAndWait();
                } else {
                    for (UMLAttribute arg : arguments)
                        newOp.addArgument(arg);
                    umlClass.addOperation(newOp);
                    result.set(true);
                    stage.close();
                }
            }
        });

        // Layout
        HBox inputLayout1 = new HBox();
        inputLayout1.getChildren().addAll(labelMod, choiceBoxMod);
        inputLayout1.setAlignment(Pos.CENTER_LEFT);
        inputLayout1.setPadding(new Insets(20));
        inputLayout1.setSpacing(20);
        HBox.setHgrow(choiceBoxMod, Priority.ALWAYS);


        HBox inputLayout2 = new HBox();
        inputLayout2.getChildren().addAll(labelName, textFieldName);
        inputLayout2.setAlignment(Pos.CENTER_LEFT);
        inputLayout2.setPadding(new Insets(20));
        inputLayout2.setSpacing(20);

        Separator separator = new Separator();
        separator.setPadding(new Insets(0, 0, 10, 0));

        HBox inputLayout3 = new HBox();
        inputLayout3.getChildren().addAll(labelType, choiceBoxType);
        inputLayout3.setAlignment(Pos.CENTER_LEFT);
        inputLayout3.setPadding(new Insets(20));
        inputLayout3.setSpacing(20);
        HBox.setHgrow(choiceBoxType, Priority.ALWAYS);

        HBox inputLayout4 = new HBox();
        inputLayout4.getChildren().addAll(labelList, checkBox);
        inputLayout4.setAlignment(Pos.CENTER_LEFT);
        inputLayout4.setPadding(new Insets(20));
        inputLayout4.setSpacing(20);
        HBox.setHgrow(choiceBoxType, Priority.ALWAYS);

        VBox argumentsLeft = new VBox();
        argumentsLeft.getChildren().addAll(labelArguments, buttonAddArgument, buttonRemoveArgument);
        argumentsLeft.setAlignment(Pos.TOP_CENTER);
        argumentsLeft.setPadding(new Insets(5, 10, 5, 10));
        argumentsLeft.setSpacing(5);

        HBox inputLayout5 = new HBox();
        inputLayout5.getChildren().addAll(argumentsLeft, argumentsRight);
        inputLayout5.setAlignment(Pos.CENTER_LEFT);
        inputLayout5.setPadding(new Insets(20));
        inputLayout5.setSpacing(20);

        VBox layout = new VBox();
        layout.getChildren().addAll(inputLayout1, inputLayout2, inputLayout3, inputLayout4, inputLayout5, separator, button);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
