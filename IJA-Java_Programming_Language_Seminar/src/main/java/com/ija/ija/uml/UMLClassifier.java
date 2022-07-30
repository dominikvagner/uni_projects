package com.ija.ija.uml;

/**
 * Represents a classifier.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class UMLClassifier extends Element {
    protected boolean isUserDefined;

    /**
     * Constructor without the need to specify the user define flag.
     * @param name Name of the classifier.
     */
    public UMLClassifier(String name) {
        super(name);
        this.isUserDefined = true;
    }

    /**
     * Constructor with all the parameters.
     * @param name Name of the classifier.
     * @param isUserDefined Is the classifier user specified.
     */
    public UMLClassifier(String name, boolean isUserDefined) {
        super(name);
        this.isUserDefined = isUserDefined;
    }

    /**
     * Create and return new not user defined classifier for specified name.
     * @param name Name of the classifier to create.
     * @return The created classifier.
     */
    public static UMLClassifier forNameClsfr(String name){
        return new UMLClassifier(name, false);
    }

    public String toString(){
        return (this.name + "(" + this.isUserDefined + ")");
    }

    // JSON requirements
    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public UMLClassifier() {
        super("");
        this.isUserDefined = true;
    }

    public boolean isUserDefined() {
        return this.isUserDefined;
    }

    public void setUserDefined(boolean userDefined) {
        this.isUserDefined = userDefined;
    }
}
