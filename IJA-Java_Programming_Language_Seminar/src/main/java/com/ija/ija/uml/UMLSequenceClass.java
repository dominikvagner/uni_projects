package com.ija.ija.uml;

/**
 * Represents a class in sequence diagram.
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class UMLSequenceClass extends Element {
    protected int start;
    protected int end;
    protected boolean error;

    /**
     * Constructor with parameters.
     * @param name Name of the class.
     * @param start From which row of sequence diagram the class starts.
     * @param end To which row of sequence diagram the class goes.
     */
    public UMLSequenceClass(String name, int start, int end) {
        super(name);
        this.start = start;
        this.end = end;
        this.error = false;
    }

    /**
     * Constructor without parameters for JSON serialization and deserialization.
     */
    public UMLSequenceClass() {
        super("");
        this.start = 0;
        this.end = 0;
        this.error = false;
    }

    public int getStart() {
        return start;
    }

    public void setStart(int start) {
        this.start = start;
    }

    public int getEnd() {
        return end;
    }

    public void setEnd(int end) {
        this.end = end;
    }

    public boolean isError() {
        return error;
    }

    public void setError(boolean error) {
        this.error = error;
    }
}
