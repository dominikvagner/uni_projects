package com.ija.ija.uml;

/** Represents a class connection in class diagram.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class UMLClassConnection {
    protected String type;
    protected String classFrom;
    protected String classTo;
    protected String cardinalityFrom;
    protected String cardinalityTo;

    /**
     * Constructor with parameters.
     * @param type Type of the connection.
     * @param classFrom Class from which the connection goes.
     * @param classTo Class to which the connection goes.
     * @param cardinalityFrom Cardinality of the connection from where it goes.
     * @param cardinalityTo Cardinality of the connection to where it goes.
     */
    public UMLClassConnection(String type, String classFrom, String classTo, String cardinalityFrom, String cardinalityTo) {
        this.type = type;
        this.classFrom = classFrom;
        this.classTo = classTo;
        this.cardinalityFrom = cardinalityFrom;
        this.cardinalityTo = cardinalityTo;
    }

    @Override
    public String toString() {
        return "Type: '" + type + '\'' +
                ", ClassFrom: '" + classFrom + '\'' +
                ", ClassTo: '" + classTo + '\'' +
                ", CardinalityFrom: '" + cardinalityFrom + '\'' +
                ", CardinalityTo: '" + cardinalityTo + '\'';
    }

    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public UMLClassConnection() {
        this.type = "";
        this.classFrom = "";
        this.classTo = "";
        this.cardinalityFrom = "";
        this.cardinalityTo = "";
    }

    public String getType() {
        return this.type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getClassFrom() {
        return classFrom;
    }

    public void setClassFrom(String classFrom) {
        this.classFrom = classFrom;
    }

    public String getClassTo() {
        return classTo;
    }

    public void setClassTo(String classTo) {
        this.classTo = classTo;
    }

    public String getCardinalityFrom() {
        return cardinalityFrom;
    }

    public void setCardinalityFrom(String cardinalityFrom) {
        this.cardinalityFrom = cardinalityFrom;
    }

    public String getCardinalityTo() {
        return cardinalityTo;
    }

    public void setCardinalityTo(String cardinalityTo) {
        this.cardinalityTo = cardinalityTo;
    }
}
