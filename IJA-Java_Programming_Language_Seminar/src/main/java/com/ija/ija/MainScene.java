package com.ija.ija;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.ija.ija.json.JsonHandler;
import com.ija.ija.json.SizedStack;
import com.ija.ija.uml.*;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.input.*;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.text.Text;
import javafx.scene.text.TextAlignment;
import javafx.scene.transform.Rotate;
import javafx.stage.Stage;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import jfxtras.labs.util.event.MouseControlUtil;
import org.jetbrains.annotations.NotNull;
import javafx.scene.shape.Polygon;

import java.awt.geom.Point2D;
import java.io.File;
import java.io.FileNotFoundException;
import java.security.cert.PolicyNode;
import java.util.ArrayList;
import java.util.List;
import java.util.Stack;
import java.util.stream.Collectors;

public class MainScene {
    //
    // Attributes:
    //
    @FXML
    public ClassDiagram classDiagram;

    public Button switch_scene;

    public Button create_diagram;

    public AnchorPane anchor;

    public ScrollPane scroll;

    protected int tabID = -1;

    protected boolean isClassViewSelected = true;

    protected Stack<String> serializedClassDiagramStack = new SizedStack<>(25);

    //
    // Functions:
    //
    public void addToStack(ClassDiagram cdToSave) {
        JsonNode saveNode = JsonHandler.toJson(cdToSave);
        try {
            this.serializedClassDiagramStack.push(JsonHandler.printNode(saveNode));
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }
    }

    public void undo() {
        if (!serializedClassDiagramStack.empty()) {
            String cdFromStack = this.serializedClassDiagramStack.pop();
            try {
                JsonNode newNode = JsonHandler.parse(cdFromStack);
                this.classDiagram = JsonHandler.fromJson(newNode, ClassDiagram.class);
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }

            if (isClassViewSelected) {
                renderClassDiagram();
            } else {
                renderSequenceDiagram();
            }
        }
    }

    public void fileLoad() {
        File file = LoadController.display();
        if (file == null) {
            return;
        }
        if (serializedClassDiagramStack.size() > 0) {
            serializedClassDiagramStack = new SizedStack<>(25);
        }

        try {
            this.classDiagram = JsonHandler.loadFromFile(file.getPath());
        } catch (FileNotFoundException ex) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setContentText("File not found.");
            alert.show();
        } catch (JsonProcessingException ex) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setContentText("JSON file in wrong format.");
            alert.show();
        }

        if (this.classDiagram.getSequenceDiagrams() != null && classDiagram.getSequenceDiagrams().size() > 0)
            this.tabID = 0;
        else
            this.tabID = -1;
        if (isClassViewSelected) {
            renderClassDiagram();
        } else {
            renderSequenceDiagram();
        }
    }

    public void fileSave() {
        File file = SaveController.display();
        if (file == null) {
            return;
        }
        if (this.classDiagram == null) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setContentText("No diagram to be saved found");
            alert.show();
        }

        boolean writeResult = JsonHandler.writeToFile(this.classDiagram, file.getPath() + "\\");

        Alert alert;
        if (writeResult) {
            alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("Diagram saved to file.");
        } else {
            alert = new Alert(Alert.AlertType.ERROR);
            alert.setContentText("Diagram couldn't be saved to file.");
        }
        alert.show();
    }

    public void switchView() {
        if (this.isClassViewSelected) {
            this.isClassViewSelected = false;
            renderSequenceDiagram();
        } else {
            this.isClassViewSelected = true;
            renderClassDiagram();
        }
    }

    public void exitProgram() {
        if (this.classDiagram != null) {
            Alert alert = new Alert(
                    Alert.AlertType.CONFIRMATION,
                    "Do you want to save the diagram before exiting?",
                    ButtonType.YES, ButtonType.NO
            );
            alert.showAndWait();

            if (alert.getResult() == ButtonType.YES) {
                fileSave();
            }
        }
        Platform.exit();
        System.exit(0);
    }

    public void showHelp() {
        Stage helpWindow = new Stage();
        helpWindow.setTitle("Help window");
        Text helpText = new Text();
        helpText.setText(
                "IJA APP - UML Diagram Editor\n\n" +
                        "Authors:\n" +
                        "Dominik Vágner, xvagne10\n" +
                        "Ladislav Vašina, xvasin11\n\n" +
                        "APP USAGE:\n" +
                        "..."
        );
        StackPane helpLayout = new StackPane();
        helpLayout.getChildren().addAll(helpText);
        Scene helpScene = new Scene(helpLayout, 400, 400);
        KeyCombination closeShortcut = new KeyCodeCombination(KeyCode.X, KeyCombination.CONTROL_DOWN);
        Runnable exitHelp = ()-> Platform.exit();
        helpScene.getAccelerators().put(closeShortcut, exitHelp);
        helpWindow.setScene(helpScene);
        helpWindow.show();
    }

    public void createDiagramButton() {
        if (isClassViewSelected) {
            if (this.classDiagram != null) {
                Alert alert = new Alert(
                        Alert.AlertType.WARNING,
                        "Creating a new class diagram when one is already loaded will result in losing " +
                                "changes made to it, if not saved first. Do you want to save the current diagram?",
                        ButtonType.YES, ButtonType.NO
                );
                alert.showAndWait();

                if (alert.getResult() == ButtonType.YES) {
                    fileSave();
                }
            }
            String cdName = CreateCDController.display();
            if (!cdName.equals("")) {
                this.classDiagram = new ClassDiagram(cdName);
                this.serializedClassDiagramStack = new SizedStack<>(10);
            }
        } else {
            if (CreateSDController.display(this.classDiagram)) {
                this.tabID++;
                renderSequenceDiagram();
            }
        }
    }

    public void addClassButton() {
        if (isClassViewSelected) {
            if (this.classDiagram == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Can't add a class to non-existing diagram. Please create one first.");
                alertError.showAndWait();
                return;
            }
            this.addToStack(this.classDiagram);
            if (AddCDClassController.display(classDiagram)) {
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        } else {
            if (this.classDiagram == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Can't add a class to non-existing diagram. Please create one first.");
                alertError.showAndWait();
                return;
            }
            this.addToStack(this.classDiagram);
            if (AddSDClassController.display(classDiagram, classDiagram.getSequenceDiagrams().get(tabID))) {
                renderSequenceDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        }
    }

    public void renderClassDiagram() {
        scroll.setContent(null);
        Stage classStage = (Stage) scroll.getScene().getWindow();
        anchor = new AnchorPane();
        scroll.setContent(anchor);

        if (this.classDiagram == null) {
            return;
        }
        classStage.setTitle("IJA APP - UML Diagram Editor : " + this.classDiagram.getName());
        for (UMLClass umlClass : this.classDiagram.getClasses()) {
            this.addTable(umlClass);
        }
        drawConnections();
    }

    public void addTable(@NotNull UMLClass umlClass) {
        // VBox
        VBox table = new VBox();
        table.computeAreaInScreen();
        table.setStyle("-fx-border-width: 2; -fx-border-color: black");

        // Class/Table name
        Label tableName = new Label(umlClass.getName());
        tableName.setStyle("-fx-text-fill: white; -fx-background-color: black; -fx-font-weight: bold");
        tableName.setAlignment(Pos.CENTER);
        tableName.setPadding(new Insets(5));
        tableName.setMaxWidth(Double.MAX_VALUE);

        // Context Menu
        ContextMenu contextMenu = new ContextMenu();
        MenuItem menuItemEdit = new MenuItem("Edit class");
        menuItemEdit.setOnAction(actionEvent -> {
            this.addToStack(this.classDiagram);
            if (EditCDClassController.display(classDiagram, umlClass)) {
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        });
        MenuItem menuItemAddAttr = new MenuItem("Add attribute");
        menuItemAddAttr.setOnAction(actionEvent -> {
            this.addToStack(this.classDiagram);
            UMLAttribute newAttr = AddCDAttributeController.display(classDiagram, umlClass, false);
            if (!newAttr.getName().equals("")) {
                umlClass.addAttribute(newAttr);
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        });
        MenuItem menuItemRemoveAttr = new MenuItem("Remove attribute");
        menuItemRemoveAttr.setOnAction(actionEvent -> {
            this.addToStack(this.classDiagram);
            if (RemoveCDAttributeController.display(umlClass.getAttributes())) {
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        });
        MenuItem menuItemAddOp = new MenuItem("Add operation");
        menuItemAddOp.setOnAction(actionEvent -> {
            this.addToStack(this.classDiagram);
            if (AddCDOperationController.display(classDiagram, umlClass)) {
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        });
        MenuItem menuItemRemoveOp = new MenuItem("Remove operation");
        menuItemRemoveOp.setOnAction(actionEvent -> {
            this.addToStack(this.classDiagram);
            if (RemoveCDOperationController.display(umlClass.getOperations())) {
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        });
        MenuItem menuItemAddConn = new MenuItem("Add connection");
        menuItemAddConn.setOnAction(actionEvent -> {
            this.addToStack(this.classDiagram);
            if (AddCDConnectionController.display(classDiagram, umlClass)) {
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        });
        MenuItem menuItemRemoveConn = new MenuItem("Remove connection");
        menuItemRemoveConn.setOnAction(actionEvent -> {
            this.addToStack(this.classDiagram);
            if (RemoveCDConnectionController.display(classDiagram, umlClass)) {
                renderClassDiagram();
            } else {
                this.serializedClassDiagramStack.pop();
            }
        });
        contextMenu.getItems().addAll(menuItemEdit, menuItemAddAttr, menuItemRemoveAttr, menuItemAddOp,
                menuItemRemoveOp, menuItemAddConn, menuItemRemoveConn);

        tableName.setContextMenu(contextMenu);
        table.getChildren().add(tableName);

        // Attributes
        for (String attr : umlClass.printAllAttributes(this.classDiagram.printAllClassifiers())) {
            boolean warningFlag = attr.endsWith("!");
            Label attrLabel = new Label(attr.replaceFirst(".$", ""));
            attrLabel.setAlignment(Pos.CENTER_LEFT);
            attrLabel.setPadding(new Insets(2.5));
            attrLabel.setMaxWidth(Double.MAX_VALUE);
            table.getChildren().add(attrLabel);
            if (warningFlag)
                attrLabel.setStyle("-fx-text-fill: red");
            if (attr.equals(umlClass.printAllAttributes(this.classDiagram.printAllClassifiers()).get(
                    umlClass.printAllAttributes(this.classDiagram.printAllClassifiers()).size() - 1))
                    && umlClass.getOperations().size() != 0) {
                attrLabel.setStyle("-fx-border-style: hidden hidden solid hidden; -fx-border-width: 1; -fx-border-color: grey");
            }
        }

        // Operations
        for (String op : umlClass.printAllOperations(classDiagram.printAllClassifiers())) {
            boolean generalizeFlag = op.endsWith("*");
            boolean warningFlag = op.endsWith("!");

            Label opLabel = new Label(op.replaceFirst(".$", ""));
            opLabel.setAlignment(Pos.CENTER_LEFT);
            opLabel.setPadding(new Insets(2.5));
            opLabel.setMaxWidth(Double.MAX_VALUE);

            if (generalizeFlag)
                opLabel.setStyle("-fx-text-fill: deepskyblue");
            if (warningFlag)
                opLabel.setStyle("-fx-text-fill: red");

            table.getChildren().add(opLabel);
        }

        // Position and dragging
        Point2D.Double layoutCords = umlClass.getCenter();
        double height = umlClass.getHeight();
        double width = umlClass.getWidth();

        table.setLayoutX(layoutCords.getX());
        table.setLayoutY(layoutCords.getY());

        MouseControlUtil.makeDraggable(table);
        table.setOnMouseReleased(mouseEvent -> {
            if (table.getLayoutX() != layoutCords.getX() || table.getLayoutY() != layoutCords.getY()
                     || table.getHeight() != height || table.getWidth() != width)
            {
                this.addToStack(this.classDiagram);
                umlClass.getCenter().setLocation(table.getLayoutX(), table.getLayoutY());
                umlClass.setHeight(table.getHeight());
                umlClass.setWidth(table.getWidth());
                umlClass.setNorth(new Point2D.Double(
                            (table.getLayoutX() + (table.getWidth() / 2)),
                            table.getLayoutY()
                        ));
                umlClass.setEast(new Point2D.Double(
                        table.getLayoutX(),
                        (table.getLayoutY() + (table.getHeight() / 2))
                ));
                umlClass.setSouth(new Point2D.Double(
                        (table.getLayoutX() + (table.getWidth() / 2)),
                        (table.getLayoutY() + table.getHeight())
                ));
                umlClass.setWest(new Point2D.Double(
                        (table.getLayoutX() + table.getWidth()),
                        (table.getLayoutY() + (table.getHeight() / 2))
                ));
                renderClassDiagram();
            }
        });

        anchor.getChildren().add(table);
    }

    private void drawConnections() {
        for (UMLClassConnection connection : this.classDiagram.getConnections()) {
            UMLClass classFrom = this.classDiagram.findClass(connection.getClassFrom());
            UMLClass classTo = this.classDiagram.findClass(connection.getClassTo());

            Point2D[] closest = this.findClosestNodes(classFrom, classTo);

            switch (connection.getType()) {
                case "Association":
                    drawArrowLine(closest[0].getX(), closest[0].getY(), closest[1].getX(), closest[1].getY(),
                            anchor, false, false, connection
                    );
                    break;
                case "Aggregation":
                    drawPolygonLine(closest[0].getX(), closest[0].getY(), closest[1].getX(), closest[1].getY(),
                            anchor, false, true, connection
                    );
                    break;
                case "Composition":
                    drawPolygonLine(closest[0].getX(), closest[0].getY(), closest[1].getX(), closest[1].getY(),
                            anchor, true, true, connection
                    );
                    break;
                case "Generalization":
                    drawPolygonLine(closest[0].getX(), closest[0].getY(), closest[1].getX(), closest[1].getY(),
                            anchor, false, false, connection
                    );
                    break;
                default:
                    break;
            }
        }
    }

    public Point2D[] findClosestNodes(UMLClass classFrom, UMLClass classTo) {
        Point2D[] closest = new Point2D[2];
        double shortestDistance = Double.MAX_VALUE;
        for (Point2D anchorBox1 : classFrom.getAnchorPoints()) {
            for (Point2D anchorBox2 : classTo.getAnchorPoints()) {
                double distance = anchorBox1.distance(anchorBox2);
                if (distance < shortestDistance) {
                    shortestDistance = distance;
                    closest[0] = anchorBox1;
                    closest[1] = anchorBox2;
                }
            }
        }
        return closest;
    }

    public void renderSequenceDiagram() {
        scroll.setContent(null);
        TabPane tabPane = new TabPane();

        if (this.classDiagram == null || this.classDiagram.getSequenceDiagrams() == null) {
            return;
        }

        int newTabID = 0;
        for (SequenceDiagram sequenceDiagram : this.classDiagram.getSequenceDiagrams()) {
            //
            // Create new tab and handle its closing
            //
            Tab tab = new Tab();
            tab.setText(sequenceDiagram.getName());
            tab.closableProperty();
            tab.setOnSelectionChanged(event -> {
                this.tabID = tabPane.getSelectionModel().getSelectedIndex();
            });
            tab.setOnCloseRequest(event -> {
                Alert alert = new Alert(
                        Alert.AlertType.CONFIRMATION,
                        "Do you really want remove this sequence diagram?",
                        ButtonType.YES, ButtonType.NO
                );
                alert.showAndWait();

                if (alert.getResult() == ButtonType.YES) {
                    this.addToStack(this.classDiagram);
                    this.tabID = -1;
                    this.classDiagram.removeSequenceDiagram(sequenceDiagram);
                } else {
                    event.consume();
                }
            });

            AnchorPane anchorPane = new AnchorPane();
            anchorPane.setMinWidth(anchor.getWidth());
            //
            // Tab contents
            //

            List<Label> labels = new ArrayList<>();
            List<Line> lines = new ArrayList<>();
            int currentColumn = 0;
            for (UMLSequenceClass sequenceClass : sequenceDiagram.getClasses()) {
                /// Label
                Label seqClassLabel = new Label(sequenceClass.getName());
                seqClassLabel.setPadding(new Insets(10));
                seqClassLabel.setLayoutX(currentColumn * 200 + 70);
                seqClassLabel.setStyle("-fx-border-color: black;");
                seqClassLabel.setLayoutY((sequenceClass.getStart() * 70 + 50));
                seqClassLabel.setMinWidth(50);
                seqClassLabel.setMinHeight(30);

                /// Context menu
                ContextMenu contextMenu = new ContextMenu();
                MenuItem menuItemAddMessage = new MenuItem("Add message.");
                int finalNewTabID = newTabID;
                menuItemAddMessage.setOnAction(mouseEvent1 -> {
                    this.addToStack(this.classDiagram);
                    if (AddSDMessageController.display(classDiagram, sequenceDiagram, sequenceClass)) {
                        renderSequenceDiagram();
                        this.tabID = finalNewTabID;
                    } else {
                        this.serializedClassDiagramStack.pop();
                    }
                });
                MenuItem menuItemRemoveMessage = new MenuItem("Remove message.");
                menuItemRemoveMessage.setOnAction(mouseEvent2 -> {
                    this.addToStack(this.classDiagram);
                    if (RemoveSDMessageController.display(classDiagram, sequenceDiagram, sequenceClass)) {
                        renderSequenceDiagram();
                        this.tabID = finalNewTabID;
                    } else {
                        this.serializedClassDiagramStack.pop();
                    }
                });
                MenuItem menuItemRemoveClass = new MenuItem("Remove class.");
                menuItemRemoveClass.setOnAction(mouseEvent3 -> {
                    this.addToStack(this.classDiagram);
                    if (sequenceDiagram.removeClass(sequenceClass)) {
                        renderSequenceDiagram();
                        this.tabID = finalNewTabID;
                    } else {
                        this.serializedClassDiagramStack.pop();
                    }
                });
                contextMenu.getItems().addAll(menuItemAddMessage, menuItemRemoveMessage, menuItemRemoveClass);
                seqClassLabel.setContextMenu(contextMenu);

                /// Class line
                double width = (seqClassLabel.getLayoutX() + (seqClassLabel.getMinWidth() / 2) + ((seqClassLabel.getText().length() - 4) * 2.5));
                double height = (seqClassLabel.getLayoutY() + seqClassLabel.getMinHeight() + 10);
                Line line = new Line();
                line.setStartX(width);
                line.setStartY(height);
                line.setEndX(width);
                // End of line from class depending on life of the class.
                if (sequenceClass.getEnd() == -1) {
                    if (sequenceClass.getStart() != 0)
                        line.setEndY(height + ((sequenceDiagram.getRowCount() - sequenceClass.getStart())) * 70);
                    else
                        line.setEndY(height + (sequenceDiagram.getRowCount() * 70));
                } else {
                    line.setEndY(height + ((sequenceClass.getEnd() - sequenceClass.getStart()) * 70 - 20));
                }
                line.getStrokeDashArray().addAll(10d, 5d);

                if (classDiagram.findClass(sequenceClass.getName()) == null) {
                    seqClassLabel.setStyle("-fx-background-color: red");
                    line.setStyle("-fx-stroke: red");
                }

                labels.add(seqClassLabel);
                lines.add(line);
                anchorPane.getChildren().addAll(seqClassLabel, line);
                currentColumn++;
            }

            int currentRow = 2;
            for (UMLSequenceConnection sequenceConnection : sequenceDiagram.getConnections()) {
                /// Arrow
                double centerFromX = lines.get(sequenceConnection.getColFrom()).getStartX();
                double centerToX;
                if (sequenceConnection.getType().equals("Creation"))
                    centerToX = labels.get(sequenceConnection.getColTo()).getLayoutX();
                else
                    centerToX = lines.get(sequenceConnection.getColTo()).getStartX();

                double fromY = (currentRow * 70);
                double toY = (currentRow * 70);
                if (sequenceConnection.getColFrom() == sequenceConnection.getColTo())
                    toY += 30;
                boolean error = classDiagram.getClasses().stream().noneMatch(n -> n.getName().equals(labels.get(sequenceConnection.getColTo()).getText())) &&
                        !sequenceConnection.getType().equals("Return");

                if (centerFromX == centerToX) {
                    Line line1 = new Line(centerFromX, fromY, centerFromX + 40, fromY);
                    Line line2 = new Line(centerFromX + 40, fromY, centerFromX + 40, toY);
                    if (error) {
                        line1.setStyle("-fx-stroke: red");
                        line2.setStyle("-fx-stroke: red");
                    }
                    anchorPane.getChildren().addAll(line1, line2);
                    drawArrowLine(centerFromX + 40, toY, centerFromX, toY, anchorPane, false, error, null);
                } else if (sequenceConnection.getType().equals("Return")) {
                    drawArrowLine(centerFromX, fromY, centerToX, toY, anchorPane, true, error, null);
                } else if (sequenceConnection.getType().equals("Destruction")) {
                    drawCrossLine(centerFromX, fromY, centerToX, toY, anchorPane);
                } else {
                    drawArrowLine(centerFromX, fromY, centerToX, toY, anchorPane, false, error, null);
                }

                /// Label
                Text msgText = new Text(sequenceConnection.getName());
                double msgWidth = msgText.prefWidth(-1);
                if (centerFromX == centerToX) {
                    msgText.setLayoutX(centerFromX + 50);
                    msgText.setLayoutY(fromY + 18);
                } else {
                    msgText.setLayoutX(((centerToX + centerFromX) / 2) - (msgWidth / 2));
                    msgText.setLayoutY(fromY - 5);
                }
                if (error) {
                    msgText.setFill(Color.RED);
                }

                anchorPane.getChildren().add(msgText);
                currentRow++;
            }

            //
            // Add tab to tabPane
            //
            anchorPane.setPadding(new Insets(0, 0, 40, 0));
            tab.setContent(anchorPane);
            tabPane.getTabs().add(tab);
            newTabID++;
        }
        if (this.tabID != -1) {
            tabPane.getSelectionModel().select(this.tabID);
        }
        tabPane.setMinWidth(10000);
        scroll.setContent(tabPane);
    }

    public static void drawArrowLine(double startX, double startY, double endX, double endY, Pane pane,
                                     boolean dashed, boolean error, UMLClassConnection connection)
    {
        // get the slope of the line and find its angle
        double slope = (startY - endY) / (startX - endX);
        double lineAngle = Math.atan(slope);

        double arrowAngle = startX > endX ? Math.toRadians(30) : -Math.toRadians(210);

        Line line = new Line(startX, startY, endX, endY);
        if (dashed)
            line.getStrokeDashArray().addAll(10d, 5d);

        double arrowLength = 10;

        // create the arrow legs
        Line arrow1 = new Line();
        arrow1.setStartX(line.getEndX());
        arrow1.setStartY(line.getEndY());
        arrow1.setEndX(line.getEndX() + arrowLength * Math.cos(lineAngle - arrowAngle));
        arrow1.setEndY(line.getEndY() + arrowLength * Math.sin(lineAngle - arrowAngle));

        Line arrow2 = new Line();
        arrow2.setStartX(line.getEndX());
        arrow2.setStartY(line.getEndY());
        arrow2.setEndX(line.getEndX() + arrowLength * Math.cos(lineAngle + arrowAngle));
        arrow2.setEndY(line.getEndY() + arrowLength * Math.sin(lineAngle + arrowAngle));

        if (error) {
            line.setStyle("-fx-stroke: red");
            arrow1.setStyle("-fx-stroke: red");
            arrow2.setStyle("-fx-stroke: red");
        }

        if (connection != null) {
            Label cardinalities = new Label(
                    "from: " + connection.getCardinalityFrom() + ", to: " + connection.getCardinalityTo()
            );
            javafx.geometry.Point2D s = new javafx.geometry.Point2D(startX, startY);
            javafx.geometry.Point2D m = s.midpoint(endX, endY);
            cardinalities.setLayoutX(m.getX());
            cardinalities.setLayoutY(m.getY());
            cardinalities.getTransforms().add(new Rotate(Math.toDegrees(lineAngle)));
            cardinalities.setTextAlignment(TextAlignment.JUSTIFY);

            pane.getChildren().addAll(line, arrow1, arrow2, cardinalities);
        } else {
            pane.getChildren().addAll(line, arrow1, arrow2);
        }
    }

    public static void drawCrossLine(double startX, double startY, double endX, double endY, @NotNull Pane pane) {
        // get the slope of the line and find its angle
        double slope = (startY - endY) / (startX - endX);
        double lineAngle = Math.atan(slope);

        double arrowAngle = startX > endX ? Math.toRadians(45) : -Math.toRadians(225);

        Line line = new Line(startX, startY, endX, endY);
        double arrowLength = 10;

        // create the cross legs
        Line arrow1 = new Line();
        arrow1.setStartX(line.getEndX());
        arrow1.setStartY(line.getEndY());
        arrow1.setEndX(line.getEndX() + arrowLength * Math.cos(lineAngle - arrowAngle));
        arrow1.setEndY(line.getEndY() + arrowLength * Math.sin(lineAngle - arrowAngle));

        Line arrow2 = new Line();
        arrow2.setStartX(line.getEndX());
        arrow2.setStartY(line.getEndY());
        arrow2.setEndX(line.getEndX() + arrowLength * Math.cos(lineAngle + arrowAngle));
        arrow2.setEndY(line.getEndY() + arrowLength * Math.sin(lineAngle + arrowAngle));

        Line arrow3 = new Line();
        arrow3.setStartX(line.getEndX() - arrowLength * Math.cos(lineAngle - arrowAngle));
        arrow3.setStartY(line.getEndY() - arrowLength * Math.sin(lineAngle - arrowAngle));
        arrow3.setEndX(line.getEndX());
        arrow3.setEndY(line.getEndY());

        Line arrow4 = new Line();
        arrow4.setStartX(line.getEndX() - arrowLength * Math.cos(lineAngle + arrowAngle));
        arrow4.setStartY(line.getEndY() - arrowLength * Math.sin(lineAngle + arrowAngle));
        arrow4.setEndX(line.getEndX());
        arrow4.setEndY(line.getEndY());

        pane.getChildren().addAll(line, arrow1, arrow2, arrow3, arrow4);
    }

    public static void drawPolygonLine(double startX, double startY, double endX, double endY, Pane pane,
                                       boolean filled, boolean rhombus, UMLClassConnection connection)
    {
        double slope = (startY - endY) / (startX - endX);
        double lineAngle = Math.atan(slope);

        double arrowAngle = startX > endX ? Math.toRadians(30) : -Math.toRadians(210);

        Line line = new Line(startX, startY, endX, endY);

        double arrowLength = 13;
        double textPadding = 20;

        if (rhombus)
            arrowLength = 10;

        Polygon polygon = new Polygon();
        polygon.getPoints().addAll(
                // 1.
                endX, endY,
                // 2.
                line.getEndX() + arrowLength * Math.cos(lineAngle - arrowAngle),
                line.getEndY() + arrowLength * Math.sin(lineAngle - arrowAngle)
        );
        if (rhombus) {
            polygon.getPoints().addAll(
                    // 3.
                    line.getEndX() + arrowLength * Math.cos(lineAngle + arrowAngle) + arrowLength * Math.cos(lineAngle - arrowAngle),
                    line.getEndY() + arrowLength * Math.sin(lineAngle + arrowAngle) + arrowLength * Math.sin(lineAngle - arrowAngle)
            );
        }
        polygon.getPoints().addAll(
                // 4.
                line.getEndX() + arrowLength * Math.cos(lineAngle + arrowAngle),
                line.getEndY() + arrowLength * Math.sin(lineAngle + arrowAngle)
        );

        if (!filled) {
            polygon.setFill(Color.web("0xF4F4F4"));
        }
        polygon.setStroke(Color.BLACK);

        Label cardinalities = new Label(
                "from: " + connection.getCardinalityFrom() + ", to: " + connection.getCardinalityTo()
        );
        javafx.geometry.Point2D s = new javafx.geometry.Point2D(startX, startY);
        javafx.geometry.Point2D m = s.midpoint(endX, endY);
        cardinalities.setLayoutX(m.getX());
        cardinalities.setLayoutY(m.getY());
        cardinalities.getTransforms().add(new Rotate(Math.toDegrees(lineAngle)));
        cardinalities.setTextAlignment(TextAlignment.JUSTIFY);

        pane.getChildren().addAll(line, polygon, cardinalities);
    }
}
