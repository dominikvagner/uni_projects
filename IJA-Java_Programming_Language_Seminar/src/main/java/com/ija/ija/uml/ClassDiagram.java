package com.ija.ija.uml;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/** Represents a class diagram.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class ClassDiagram extends Element {
    protected List<UMLClassifier> classifiers;
    protected List<UMLClass> classes;
    protected List<UMLClassConnection> connections;
    protected List<String> accessMods;
    protected List<String> connTypes;
    protected List<String> connCardinalities;
    protected List<SequenceDiagram> sequenceDiagrams;

    /**
     * Constructor for class diagram with specified name.
     * @param name Name of the class diagram.
     */
    public ClassDiagram(java.lang.String name) {
        super(name);
        this.classes = new ArrayList<>();
        this.classifiers = new ArrayList<>();
        this.connections = new ArrayList<>();
        this.accessMods = new ArrayList<>(Arrays.asList("-", "#", "+", "~"));
        this.connTypes = new ArrayList<>(Arrays.asList("Association", "Aggregation", "Composition", "Generalization"));
        this.connCardinalities = new ArrayList<>(Arrays.asList("1", "0...1", "*", "1...*"));
        this.sequenceDiagrams = new ArrayList<>();
        addDefaultTypes();
    }

    /**
     * Create a class and add it to classes and classifiers.
     * @param name Name of the class.
     * @param isAbstract Is class abstract.
     * @return The created class.
     */
    public UMLClass createClass(String name, Boolean isAbstract) {
        UMLClass class_add = new UMLClass(name, isAbstract);
        if (this.classes.stream().anyMatch(n -> n.getName().equals(class_add.getName()))) {
            return null;
        }
        this.classes.add(class_add);
        UMLClassifier new_cls_classifier = new UMLClassifier(name, true);
        this.classifiers.add(new_cls_classifier);
        return class_add;
    }

    /**
     * Remove a Class and check for its existence in sequence diagrams and mark in them that error occurred.
     * @param name Name of the class to remove.
     * @return Boolean representing the operation success.
     */
    public boolean removeClass(String name) {
        UMLClass umlClass = this.findClass(name);
        if (umlClass == null)
            return false;
        for (SequenceDiagram seqDia : this.sequenceDiagrams ) {
            UMLSequenceClass sequenceClass = seqDia.findClass(name);
            if (sequenceClass == null)
                break;
            sequenceClass.setError(true);
        }
        this.classes.remove(umlClass);
        this.classifiers.remove(this.findClassifier(name));
        return true;
    }

    /**
     * Remove a sequence diagram from the list of sequence diagrams.
     * @param sequenceDiagram Sequence diagram to remove.
     * @return Boolean representing the operation success.
     */
    public boolean removeSequenceDiagram(SequenceDiagram sequenceDiagram) {
        if (!this.sequenceDiagrams.contains(sequenceDiagram)) {
            return false;
        }
        this.sequenceDiagrams.remove(sequenceDiagram);
        return true;
    }

    /**
     * Renaming a class and its occurrences.
     * @param oldName Name of the class to rename.
     * @param newName New name of the class.
     * @return Boolean representing the operation success.
     */
    public boolean renameClass(String oldName, String newName) {
        UMLClass cls = findClass(oldName);
        if (cls == null)
            return false;
        cls.setName(newName);
        UMLClassifier classClassifier = this.findClassifier(oldName);
        classClassifier.setName(newName);
        for (UMLClassConnection conn : this.connections) {
            if (conn.classFrom.equals(oldName)) {
                conn.setClassFrom(newName);
            }
            if (conn.classTo.equals(oldName)) {
                conn.setClassTo(newName);
            }
        }
        for (SequenceDiagram seqDia : this.sequenceDiagrams ) {
            UMLSequenceClass sequenceClass = seqDia.findClass(oldName);
            if (sequenceClass == null)
                return false;
            sequenceClass.setName(newName);
        }
        return true;
    }

    /**
     * Find a classifier in class diagram, if not found create one with the specified name.
     * @param name Name of the wanted classifier.
     * @return The found or created classifier.
     */
    public UMLClassifier classifierForName(String name) {
        UMLClassifier find = findClassifier(name);
        if (find != null) { return find; }
        UMLClassifier classifier = UMLClassifier.forNameClsfr(name);
        this.classifiers.add(classifier);
        return classifier;
    }

    /**
     * Find a classifier in class diagram.
     * @param name Name of the wanted classifier.
     * @return The found classifier.
     */
    public UMLClassifier findClassifier(String name) {
        for (UMLClassifier classifier : this.classifiers) {
            if (classifier.getName().equals(name)) {
                return classifier;
            }
        }
        return null;
    }

    /**
     * Find a class in class diagram.
     * @param name Name of the wanted class.
     * @return The found class.
     */
    public UMLClass findClass(String name) {
        for (UMLClass clas : this.classes) {
            if (clas.getName().equals(name)) {
                return clas;
            }
        }
        return null;
    }

    /**
     * Add default types of classifiers in class diagram.
     */
    public void addDefaultTypes() {
        String[] defaultTypes = new String[] {"int", "long", "float", "double", "string", "boolean", "void"};
        for (String type : defaultTypes)
            classifierForName(type);
    }

    /**
     * Add connection between classes in class diagram.
     * @param type Type of connection.
     * @param classFrom Class the connection is from.
     * @param classTo Class to which the connection goes.
     * @param cardinalityFrom Cardinality of the connection from a class.
     * @param cardinalityTo Cardinality of the connection to a class.
     */
    public void addConnection(String type, String classFrom, String classTo, String cardinalityFrom, String cardinalityTo) {
        UMLClassConnection conn = new UMLClassConnection(type, classFrom, classTo, cardinalityFrom, cardinalityTo);
        this.connections.add(conn);
        if (type.equals("Generalization")) {
            addGeneralization(classFrom, classTo);
        }
    }

    /**
     * Add a generalization type connection.
     * @param classFrom Class from which the connection goes.
     * @param classTo Class to which the connection goes.
     */
    public void addGeneralization(String classFrom, String classTo) {
        UMLClass from = findClass(classFrom);
        UMLClass to = findClass(classTo);
        List<UMLOperation> generalizedOperations = new ArrayList<>();
        for (UMLOperation op : to.getGeneralizedOperations()) {
            if (!op.toString().startsWith("-")) {
                generalizedOperations.add(op);
            }
        }
        for (UMLOperation op : to.getOperations()) {
            if (!op.toString().startsWith("-")) {
                generalizedOperations.add(op);
            }
        }
        List<UMLAttribute> generalizedAttributes = new ArrayList<>();
        for (UMLAttribute attr : to.getGeneralizedAttributes()) {
            if (!attr.toString().startsWith("-")) {
                generalizedAttributes.add(attr);
            }
        }
        for (UMLAttribute attr : to.getAttributes()) {
            if (!attr.toString().startsWith("-")) {
                generalizedAttributes.add(attr);
            }
        }
        from.setGeneralizedAttributes(generalizedAttributes);
        from.setGeneralizedOperations(generalizedOperations);
    }

    /**
     * Create a sequence diagram, and add it to class diagram.
     * @param name Name of the sequence diagram.
     * @return The created sequence diagram.
     */
    public SequenceDiagram createSeqDiagram(String name) {
        SequenceDiagram seqDia = new SequenceDiagram(name);
        if (this.sequenceDiagrams.stream().anyMatch(n -> n.getName().equals(seqDia.getName()))) {
            return null;
        }
        this.sequenceDiagrams.add(seqDia);
        return seqDia;
    }

    /**
     * Create a list of strings representing the diagrams classifiers.
     * @return List of classifier names.
     */
    public List<String> printAllClassifiers() {
        List<String> result = new ArrayList<>();
        for (UMLClassifier classifier : this.classifiers) {
            String tmp = classifier.toString();
            int pos = tmp.indexOf("(");
            if (pos != -1)
                result.add(tmp.substring(0, pos));
        }
        return result;
    }

    public SequenceDiagram findSequenceDiagram(String name) {
        for (SequenceDiagram sequenceDiagram : this.sequenceDiagrams) {
            if (sequenceDiagram.getName().equals(name)) {
                return sequenceDiagram;
            }
        }
        return null;
    }

    // JSON requirements

    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public ClassDiagram() {
        super("");
        this.classes = new ArrayList<>();
        this.classifiers = new ArrayList<>();
        this.connections = new ArrayList<>();
        this.accessMods = new ArrayList<>(Arrays.asList("-", "#", "+", "~"));
        this.connTypes = new ArrayList<>(Arrays.asList("Association", "Aggregation", "Composition", "Generalization"));
        this.connCardinalities = new ArrayList<>(Arrays.asList("1", "0...1", "*", "1...*"));
        this.sequenceDiagrams = new ArrayList<>();
    }

    public List<UMLClassifier> getClassifiers() {
        return classifiers;
    }

    public void setClassifiers(List<UMLClassifier> classifiers) {
        this.classifiers = classifiers;
    }

    public List<UMLClass> getClasses() {
        return classes;
    }

    public void setClasses(List<UMLClass> classes) {
        this.classes = classes;
    }

    public List<String> getAccessMods() {
        return accessMods;
    }

    public void setAccessMods(List<String> accessMods) {
        this.accessMods = accessMods;
    }

    public List<String> getConnTypes() {
        return connTypes;
    }

    public void setConnTypes(List<String> connTypes) {
        this.connTypes = connTypes;
    }

    public List<String> getConnCardinalities() {
        return connCardinalities;
    }

    public void setConnCardinalities(List<String> connCardinalities) {
        this.connCardinalities = connCardinalities;
    }

    public List<UMLClassConnection> getConnections() {
        return connections;
    }

    public void setConnections(List<UMLClassConnection> connections) {
        this.connections = connections;
    }

    public List<SequenceDiagram> getSequenceDiagrams() {
        return sequenceDiagrams;
    }

    public void setSequenceDiagrams(List<SequenceDiagram> sequenceDiagrams) {
        this.sequenceDiagrams = sequenceDiagrams;
    }
}
