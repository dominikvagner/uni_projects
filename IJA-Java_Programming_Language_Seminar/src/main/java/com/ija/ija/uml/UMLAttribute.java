package com.ija.ija.uml;

/**
 * Represents an attribute in class.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class UMLAttribute extends Element {
    protected UMLClassifier type;
    protected String modifier;
    protected boolean isList;

    /**
     * Constructor with parameters.
     * @param name Name of the attribute.
     * @param type Type of the attribute.
     * @param modifier Modifier of the attribute.
     */
    public UMLAttribute(String name, UMLClassifier type, String modifier, boolean isList) {
        super(name);
        this.type = type;
        this.modifier = modifier;
        this.isList = isList;
    }

    @Override
    public String toString() {
        String result = this.modifier + " " + this.name + " : ";
        if (this.isList)
            result += "List<" + this.type.getName() + ">";
        else
            result += this.type.getName();
        return result;
    }

    /**
     * toString() without the access modifier.
     * @return String result.
     */
    public String modlessToString() {
        String result = this.name + " : ";
        if (this.isList)
            result += "List<" + this.type.getName() + ">";
        else
            result += this.type.getName();
        return result;
    }

    // JSON requirements
    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public UMLAttribute() {
        super("");
        this.type = new UMLClassifier();
        this.modifier = "-";
        this.isList = false;
    }

    public UMLClassifier getType() {
        return this.type;
    }

    public void setType(UMLClassifier type) {
        this.type = type;
    }

    public String getModifier() {
        return modifier;
    }

    public void setModifier(String modifier) {
        this.modifier = modifier;
    }

    public boolean isList() {
        return isList;
    }

    public void setList(boolean list) {
        isList = list;
    }
}
