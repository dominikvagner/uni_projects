package com.ija.ija.uml;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

/**
 * Represents and operation in class.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class UMLOperation extends UMLAttribute {
    protected List<UMLAttribute> arguments;

    /**
     * Constructor with parameters.
     * @param name Name of the operation.
     * @param type Type of the operation.
     * @param modifier Modifier of the operation.
     */
    public UMLOperation(String name, UMLClassifier type, String modifier, boolean isList) {
        super(name, type, modifier, isList);
        this.arguments = new ArrayList<>();
    }

    /**
     * Create operation with a variable amount of attributes.
     * @param name Name of the operation.
     * @param type Type of the operation.
     * @param modifier Modifiers of the operation.
     * @param args Variable amount of arguments that the operation has.
     * @return The created operation.
     */
    public static UMLOperation create(String name, UMLClassifier type, String modifier, boolean isList, UMLAttribute... args){
        UMLOperation newOp = new UMLOperation(name, type, modifier, isList);
        newOp.arguments.addAll(Arrays.asList(args));
        return newOp;
    }

    /**
     * Add attribute to operation.
     * @param arg Attribute to add.
     * @return Boolean representing the operation success.
     */
    public boolean addArgument(UMLAttribute arg){
        if (this.arguments.stream().anyMatch(n -> n.getName().equals(arg.getName()))) {
            return false;
        }
        return this.arguments.add(arg);
    }

    @Override
    public String toString() {
        String result = "";
        result += this.modifier + " " + this.name + "(";
        for (UMLAttribute attr : this.arguments) {
            result += attr.modlessToString();
            if (this.arguments.indexOf(attr) != (this.arguments.size() - 1))
                result += ", ";
        }
        if (this.isList)
            result += (") : List<" + this.type.getName() + ">");
        else
            result += (") : " + this.type.getName());
        return result;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        UMLOperation that = (UMLOperation) o;
        return Objects.equals(arguments, that.arguments);
    }

    @Override
    public int hashCode() {
        return Objects.hash(arguments);
    }

    // JSON requirements
    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public UMLOperation() {
        super("", new UMLClassifier(), "-", false);
        this.arguments = new ArrayList<>();
    }

    public List<UMLAttribute> getArguments() {
        return this.arguments;
    }

    public void setArguments(List<UMLAttribute> arguments) {
        this.arguments = arguments;
    }
}
