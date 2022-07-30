package com.ija.ija.uml;

import java.awt.geom.Point2D;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Represents a class in class diagram.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class UMLClass extends UMLClassifier {
    protected boolean isAbstract;
    protected List<UMLAttribute> attributes;
    protected List<UMLOperation> operations;
    protected List<UMLAttribute> generalizedAttributes;
    protected List<UMLOperation> generalizedOperations;
    protected Point2D.Double center;
    protected Point2D.Double north;
    protected Point2D.Double east;
    protected Point2D.Double south;
    protected Point2D.Double west;
    protected double height;
    protected double width;

    /**
     * Constructor with parameters.
     * @param name Name of the new class.
     * @param isAbstract Flag if the class is abstract.
     */
    public UMLClass(String name, Boolean isAbstract) {
        super(name, true);
        this.attributes = new ArrayList<>();
        this.operations = new ArrayList<>();
        this.generalizedAttributes = new ArrayList<>();
        this.generalizedOperations = new ArrayList<>();
        this.isAbstract = isAbstract;
        this.center = new Point2D.Double();
        this.north = new Point2D.Double();
        this.east = new Point2D.Double();
        this.south = new Point2D.Double();
        this.west = new Point2D.Double();
        this.center.setLocation(50, 30);
        this.height = 0;
        this.width = 0;
    }

    /**
     * Add attribute to class.
     * @param attr The attribute to add.
     * @return Boolean representing the operation success.
     */
    public boolean addAttribute(UMLAttribute attr){
        if (this.attributes.stream().anyMatch(n -> n.getName().equals(attr.getName()))) {
            return false;
        }
        return this.attributes.add(attr);
    }

    /**
     * Add attribute to class.
     * @param operation The operation to add.
     * @return Boolean representing the operation success.
     */
    public boolean addOperation(UMLOperation operation){
        if (this.operations.stream().anyMatch(n -> n.getName().equals(operation.getName()))) {
            return false;
        }
        return this.operations.add(operation);
    }

    /**
     * Return a list of strings for all the class attributes.
     * @return List of string results.
     */
    public List<String> printAllAttributes(List<String> classifierList) {
        List<String> result = new ArrayList<>();
        for (UMLAttribute attr : this.attributes) {
            result.add(this.printAttribute(attr, classifierList));
        }
        return result;
    }

    /**
     * Get string representation of an attribute.
     * @param attr Attribute to represent.
     * @return The string result.
     */
    private String printAttribute(UMLAttribute attr, List<String> classifierList) {
        String result;
        if (this.generalizedAttributes.stream().anyMatch(n -> n.getName().equals(attr.getName()))) {
            result = attr.toString() + "!";
        } else if (!classifierList.contains(attr.getType().getName())) {
            result = attr.toString() + "!";
        } else {
            result = attr.toString() + " ";
        }
        return result;
    }

    /**
     * Return a list of strings for all the class operations.
     * @return List of string results.
     */
    public List<String> printAllOperations(List<String> classifierList) {
        List<String> result = new ArrayList<>();
        for (UMLOperation op : this.operations) {
            result.add(this.printOperation(op, classifierList));
        }
        return result;
    }

    /**
     * Get string representation of an operation.
     * @param op Operation to represent.
     * @param classifierList List of class diagram classifiers.
     * @return The string result.
     */
    public String printOperation(UMLOperation op, List<String> classifierList) {
        String result;
        boolean error = false;
        if (this.generalizedOperations.stream().anyMatch(n -> n.getName().equals(op.getName()))) {
            UMLOperation tmpOp = this.findGeneralizedOperation(op.getName());
            boolean matches = true;
            if (tmpOp.getArguments().size() == op.getArguments().size()) {
                for (int i = 0; i < tmpOp.getArguments().size(); i++) {
                    if (!classifierList.contains(op.getArguments().get(i).getType().getName())) {
                        error = true;
                        break;
                    }
                    if (!tmpOp.getArguments().get(i).getType().getName().equals(op.getArguments().get(i).getType().getName())) {
                        matches = false;
                    }
                }
            } else {
                matches = false;
            }
            result = op.toString();
            if (op.getName().equals(this.getName())) {
                result = op.toString().substring(0, result.indexOf(')') + 1);
                System.out.println(result);
            }
            if (error) {
                result += "!";
            } else if (matches) {
                result += "*";
            } else {
                result += " ";
            }
        } else {
            for (int i = 0; i < op.getArguments().size(); i++) {
                if (!classifierList.contains(op.getArguments().get(i).getType().getName())) {
                    error = true;
                    break;
                }
            }
            result = op.toString();
            if (op.getName().equals(this.getName())) {
                result = op.toString().substring(0, result.indexOf(')') + 1);
            }
            if (error) {
                result += "!";
            } else {
                result += " ";
            }
        }
        return result;
    }

    /**
     * Find an operation in generalized operations.
     * @param opName Name of the wanted operation.
     * @return The found operation.
     */
    public UMLOperation findGeneralizedOperation(String opName) {
        for (UMLOperation op : this.generalizedOperations) {
            if (op.getName().equals(opName)) {
                return op;
            }
        }
        return null;
    }

    public List<Point2D.Double> getAnchorPoints() {
        return new ArrayList<>(Arrays.asList(this.north, this.east, this.south, this.west));
    }

    @Override
    public String toString() {
        return name;
    }

    // JSON requirements
    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public UMLClass() {
        super("", true);
        this.attributes = new ArrayList<>();
        this.operations = new ArrayList<>();
        this.generalizedAttributes = new ArrayList<>();
        this.generalizedOperations = new ArrayList<>();
        this.center = new Point2D.Double();
        this.north = new Point2D.Double();
        this.east = new Point2D.Double();
        this.south = new Point2D.Double();
        this.west = new Point2D.Double();
        this.center.setLocation(50, 30);
        this.height = 0;
        this.width = 0;
    }

    public boolean isAbstract() {
        return isAbstract;
    }

    public void setAbstract(boolean anAbstract) {
        this.isAbstract = anAbstract;
    }

    public List<UMLAttribute> getAttributes() {
        return this.attributes;
    }

    public void setAttributes(List<UMLAttribute> attributes) {
        this.attributes = attributes;
    }

    public List<UMLOperation> getOperations() {
        return this.operations;
    }

    public void setOperations(List<UMLOperation> operations) {
        this.operations = operations;
    }

    public List<UMLAttribute> getGeneralizedAttributes() {
        return generalizedAttributes;
    }

    public void setGeneralizedAttributes(List<UMLAttribute> generalizedAttributes) {
        this.generalizedAttributes = generalizedAttributes;
    }

    public List<UMLOperation> getGeneralizedOperations() {
        return generalizedOperations;
    }

    public void setGeneralizedOperations(List<UMLOperation> generalizedOperations) {
        this.generalizedOperations = generalizedOperations;
    }

    public Point2D.Double getCenter() {
        return center;
    }

    public void setCenter(Point2D.Double center) {
        this.center = center;
    }

    public Point2D.Double getNorth() {
        return north;
    }

    public void setNorth(Point2D.Double north) {
        this.north = north;
    }

    public Point2D.Double getEast() {
        return east;
    }

    public void setEast(Point2D.Double east) {
        this.east = east;
    }

    public Point2D.Double getSouth() {
        return south;
    }

    public void setSouth(Point2D.Double south) {
        this.south = south;
    }

    public Point2D.Double getWest() {
        return west;
    }

    public void setWest(Point2D.Double west) {
        this.west = west;
    }

    public double getHeight() {
        return height;
    }

    public void setHeight(double height) {
        this.height = height;
    }

    public double getWidth() {
        return width;
    }

    public void setWidth(double width) {
        this.width = width;
    }
}
