package com.ija.ija.uml;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/** Represents a sequence diagram.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class SequenceDiagram extends Element {
    protected int rowCount;
    protected int colCount;
    protected List<UMLSequenceClass> classes;
    protected List<UMLSequenceConnection> connections;
    protected List<String> connectionTypes;

    /**
     * Constructor with specified name of the diagram.
     * @param name Name of the diagram.
     */
    public SequenceDiagram(String name) {
        super(name);
        this.rowCount = 1;
        this.colCount = 0;
        this.classes = new ArrayList<>();
        this.connections = new ArrayList<>();
        this.connectionTypes = new ArrayList<>(Arrays.asList("Synchronous", "Asynchronous", "Return", "Creation", "Destruction"));
    }

    /**
     * Add a sequence class to sequence diagram.
     * @param className Name of the ne class.
     * @param isConnectionCreated Is created by sequence connection.
     * @return Boolean representing the operation success.
     */
    public boolean addClass(String className, boolean isConnectionCreated) {
        if (this.classes.stream().anyMatch(n -> n.getName().equals(className))) {
            return false;
        }
        UMLSequenceClass sequenceClass;
        if (isConnectionCreated) {
            sequenceClass = new UMLSequenceClass(className, this.getRowCount(), -1);
        } else {
            sequenceClass = new UMLSequenceClass(className, 0, -1);
        }
        this.colCount++;
        return classes.add(sequenceClass);
    }

    /**
     * Function to remove a class from sequence diagram.
     * @param cls Sequence class to remove
     * @return Boolean representing the operation success.
     */
    public boolean removeClass(UMLSequenceClass cls) {
        int classPos = this.classes.indexOf(cls);
        List<UMLSequenceConnection> connectionsToRemove = new ArrayList<>();
        for (UMLSequenceConnection connectionToRemove : this.connections) {
            if (connectionToRemove.colFrom == classPos || connectionToRemove.colTo == classPos) {
                connectionsToRemove.add(connectionToRemove);
            }
        }
        for (UMLSequenceConnection connectionToRemove : connectionsToRemove) {
            removeConnection(connectionToRemove);
        }
        for (UMLSequenceConnection connectionToAdjust : this.connections) {
            if (connectionToAdjust.colFrom > classPos) {
                connectionToAdjust.colFrom--;
            }
            if (connectionToAdjust.colTo > classPos) {
                connectionToAdjust.colTo--;
            }
        }
        this.classes.remove(cls);
        this.colCount--;
        return true;
    }

    /**
     * Get a position of sequence class in sequence diagram.
     * @param className Name of the class.
     * @return Position of the class.
     */
    public int classPosition(String className) {
        for (UMLSequenceClass cls : this.classes) {
            if (cls.getName().equals(className)) {
                return this.classes.indexOf(cls);
            }
        }
        return -1;
    }

    /**
     * Find a class in the diagram.
     * @param className Name of the wanted class.
     * @return The found sequence class.
     */
    public UMLSequenceClass findClass(String className) {
        for (UMLSequenceClass cls : this.classes) {
            if (cls.getName().equals(className)) {
                return cls;
            }
        }
        return null;
    }

    /**
     * Add a sequence connection between classes.
     * @param type Type of the connection.
     * @param label Label of the connection.
     * @param classFrom Class from which the connection goes.
     * @param classTo Class to which the connection goes.
     * @return Boolean representing the operation success.
     */
    public boolean addConnection(String type, String label, String classFrom, String classTo) {
        UMLSequenceConnection seqConn;
        int colFrom = 0;
        int colTo = 0;
        switch (type) {
            case "Synchronous":
            case "Asynchronous":
            case "Return":
                colFrom = this.classPosition(classFrom);
                colTo = this.classPosition(classTo);
                break;
            case "Creation":
                colFrom = this.classPosition(classFrom);
                this.addClass(classTo, true);
                colTo = this.classPosition(classTo);
                break;
            case "Destruction":
                colFrom = this.classPosition(classFrom);
                UMLSequenceClass classToDestroy = findClass(classTo);
                classToDestroy.setEnd(this.rowCount);
                colTo = this.classPosition(classTo);
                break;
            default:
                return false;
        }
        seqConn = new UMLSequenceConnection(label, type, colFrom, colTo);
        this.connections.add(seqConn);
        this.rowCount++;
        return true;
    }

    /**
     * Remove a sequence connection from diagram.
     * @param connection The connection to remove.
     * @return Boolean representing the operation success.
     */
    public boolean removeConnection(UMLSequenceConnection connection) {
        switch (connection.type) {
            case "Synchronous":
            case "Asynchronous":
            case "Return":
                this.connections.remove(connection);
                break;
            case "Creation":
                int classPos = connection.colTo;
                UMLSequenceClass classToDelete = this.classes.get(classPos);
                this.connections.remove(connection);
                for (UMLSequenceConnection connectionToDelete : this.connections) {
                    if (connectionToDelete.colFrom == classPos || connectionToDelete.colTo == classPos) {
                        removeConnection(connectionToDelete);
                    }
                }
                for (UMLSequenceConnection connectionToAdjust : this.connections) {
                    if (connectionToAdjust.colFrom > classPos) {
                        connectionToAdjust.colFrom--;
                    }
                    if (connectionToAdjust.colTo > classPos) {
                        connectionToAdjust.colTo--;
                    }
                }
                this.classes.remove(classToDelete);
                this.colCount--;
                break;
            case "Destruction":
                UMLSequenceClass cls = this.classes.get(connection.colTo);
                cls.setEnd(-1);
                this.connections.remove(connection);
                break;
            default:
                return false;
        }
        this.rowCount--;
        return true;
    }

    // JSON Requirements
    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public SequenceDiagram() {
        super("");
        this.rowCount = 1;
        this.colCount = 0;
        this.classes = new ArrayList<>();
        this.connections = new ArrayList<>();
        this.connectionTypes = new ArrayList<>(Arrays.asList("Synchronous", "Asynchronous", "Return", "Creation", "Destruction"));
    }

    public int getRowCount() {
        return rowCount;
    }

    public void setRowCount(int rowCount) {
        this.rowCount = rowCount;
    }

    public int getColCount() {
        return colCount;
    }

    public void setColCount(int colCount) {
        this.colCount = colCount;
    }

    public List<UMLSequenceClass> getClasses() {
        return classes;
    }

    public void setClasses(List<UMLSequenceClass> classes) {
        this.classes = classes;
    }

    public List<UMLSequenceConnection> getConnections() {
        return connections;
    }

    public void setConnections(List<UMLSequenceConnection> connections) {
        this.connections = connections;
    }

    public List<String> getConnectionTypes() {
        return connectionTypes;
    }

    public void setConnectionTypes(List<String> connectionTypes) {
        this.connectionTypes = connectionTypes;
    }
}
