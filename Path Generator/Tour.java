/**
 * This class creates a Tour of Points using a 
 * Linked List implementation.  The points can
 * be inserted into the list using two heuristics.
 * @author Eliot Huh, Grant Lee
 * @author Layla Oesper, modified code 09-22-2017
 */

// ADD YOUR METHODS BELOW HERE
public class Tour {

    /** A helper class that defines a single node for use in a tour.
     * A node consists of a Point, representing the location of that
     * city in the tour, and a pointer to the next Node in the tour.
     */
    
    private class Node {
        private Point p;
        private Node next;
	
        /** Constructor creates a new Node at the given Point newP
         * with an intital next value of null.
         * @param newP - the point to associate with the Node.
         */
        public Node(Point newP) {
            p = newP;
            next = null;
        }

        /** Constructor creates a new Node at the given Point newP
         * with the specified next node.
         * @param newP - the point to associate with the Node.
         * @param nextNode - the nextNode this node should point to.
         */
        public Node(Point newP, Node nextNode) {
            p = newP;
            next = nextNode;
        }
	
        /**
         * Return the Point associated with this Node. 
         * (Same value can also be accessed from a Node object node
         * using node.p)
         * @return The Point object associated with this node.
         */
        public Point getPoint() {
            return p;
        }
        
        /**
         * Return the next Node associated with this Node. 
         * (Same value can also be accessed from a Node object node
         * using node.next)
         * @return The next Node object linked from this node.
         */
	   public Node getNext() {
	       return next;
	   }
          
    } // End Node class
    

    // Tour class Instance variables
    private Node head;
    private int size; //number of nodes
    //Add other instance variables you think might be useful.
    
    
    /**
     * Constructor for the Tour class.  By default sets head to null.
     */
    public Tour() {
        head = null;
        size = 0;
    }
    
    /**
     * Formats the linked list into a String
     * @return the linked list as a String 
     */ 
    public String toString() { 
        Node current = head;
        String list = head.p.toString() + "\n";
        while(current.next != null) {
            list += current.next.p.toString() + "\n";
            current = current.next;
        } 
        return list; 
    }
    
    /**
     * Draws each point and connects the points in the order they link in the list, with the  
     * last point connecting to the first 
     */ 
    public void draw() {
        Node current = head; 
        while(current.next != null) {
            current.p.draw(); 
            current.p.drawTo(current.next.p);
            current = current.next;
        } 
        current.p.drawTo(head.p);
    }
    
    /**
     * Returns the number of nodes in the list
     * @return the number of nodes in the list as an int
     */ 
    public int size() {
        return size;
    }
    
    /**
    * Loops through all the nodes and sums up the total distance between nodes
    * @return the total distance from each point to the next point as double
    */
    public double distance() {
        double distance = 0;
        if(head != null) {
            Node current = head;
            while(current.next != null) {
                distance += current.p.distanceTo(current.next.p);
                current = current.next;
            } 
            distance += current.p.distanceTo(head.p);
            return distance;
        }
        else{
            return 0;
        }
    }
    
    /** 
    * Insert a new node after the node that is closest to the given point
    * @param P is the point that is inserted
    */
    public void insertNearest(Point P) {
        double minDist = Double.MAX_VALUE; 
        Node closest = null; 
        Node current = head;
        if(head != null) {
            while(current.next != null) {
                if(P.distanceTo(current.p) < minDist) {
                    closest = current;
                    minDist = P.distanceTo(current.p);
                }
                current = current.next;
            } 
            if (P.distanceTo(current.p) < minDist) {
                System.out.println(2);
                closest = current;
                Node newInsert = new Node(P, null);
                closest.next = newInsert; 
                size++;
                return;
            } 
            else {
                System.out.println(1);
                Node newInsert = new Node(P, closest.next);
                closest.next = newInsert; 
            }
        }
        else {
            System.out.println(4);
            head = new Node(P, null);
        }
        size++;
    }
    
    /** 
    * Insert a new node after the saved node associated with smallestDist
    * @param P is the point that is inserted
    */
    public void insertSmallest(Point P) {
        double smallestDist = Double.MAX_VALUE;
        Node current = head;
        double totalDist = this.distance();
        double tempDist = 0;
        Node lilP = null;
        
        if(head != null) {
            while(current.next != null) {
                tempDist = totalDist - current.p.distanceTo(current.next.p) + current.p.distanceTo(P) + P.distanceTo(current.next.p);                             
                if(tempDist < smallestDist) {
                    smallestDist = tempDist;
                    lilP = current;
                }
                current = current.next;
            }
            tempDist = totalDist - current.p.distanceTo(head.p) + current.p.distanceTo(P) + P.distanceTo(head.p);
            if(tempDist < smallestDist) {
                Node insertP = new Node(P, null);
                current.next = insertP;
                size++;
                return;
            }
            Node insertP = new Node(P, lilP.next);
            lilP.next = insertP;
        }
        else {
            head = new Node(P, null);
        }
        size++;
    }
    
    
    
    // ADD YOUR METHODS ABOVE HERE
   
    public static void main(String[] args) {
        /**Use your main() function to test your code as you write it. 
        This main() will not actually be run once you have the entire
        Tour class complete, instead you will run the NearestInsertion
        and SmallestInsertion programs which call the functions in this 
        class. 
        */
        
        //One example test could be the follow (uncomment to run):
        Tour tour = new Tour();
        Point p = new Point(0,0);
        tour.insertNearest(p);
        p = new Point(0,100);
        tour.insertNearest(p);
        p = new Point(100, 100);
        tour.insertNearest(p);
        System.out.println("Tour distance =  "+tour.distance());
        System.out.println("Number of points = "+tour.size());
        System.out.println(tour);
         

        // the tour size should be 3 and the distance 341.42 (don't forget to include the trip back
        // to the original point)
    
        // uncomment the following section to draw the tour, setting w and h to the max x and y 
        // values that occur in your tour points
	
        int w = 100 ; //Set this value to the max that x can take on
        int h = 100 ; //Set this value to the max that y can take on
        StdDraw.setCanvasSize(w, h);
        StdDraw.setXscale(0, w);
        StdDraw.setYscale(0, h);
        StdDraw.setPenRadius(.005);
        tour.draw(); 
    }
   
}