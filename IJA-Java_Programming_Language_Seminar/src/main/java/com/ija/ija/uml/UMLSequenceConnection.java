package com.ija.ija.uml;

/**
 * Represents a connection in sequence diagram.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class UMLSequenceConnection extends Element {
    protected String type;
    protected int colFrom;
    protected int colTo;

    /**
     * Constructor with parameters.
     * @param name Name of the connection.
     * @param type Type of the connection.
     * @param colFrom From which column/class which the connection starts.
     * @param colTo To which column/class which the connection goes.
     */
    public UMLSequenceConnection(String name, String type, int colFrom, int colTo) {
        super(name);
        this.type = type;
        this.colFrom = colFrom;
        this.colTo = colTo;
    }

    @Override
    public String toString() {
        return "name='" + name + '\'' +
                ", type='" + type + '\'' +
                ", colFrom=" + colFrom +
                ", colTo=" + colTo;
    }

    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public UMLSequenceConnection() {
        super("");
        this.type = "";
        this.colFrom = 0;
        this.colTo = 0;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public int getColFrom() {
        return colFrom;
    }

    public void setColFrom(int colFrom) {
        this.colFrom = colFrom;
    }

    public int getColTo() {
        return colTo;
    }

    public void setColTo(int colTo) {
        this.colTo = colTo;
    }
}
