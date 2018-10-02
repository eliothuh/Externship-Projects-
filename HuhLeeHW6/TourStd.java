/*************************************************************************
 *  Initial start to a class that implements 2 heuristic algorithms for
 *  the traveling salesman problem, nearest-neighbor and smallest-distance.

   code by Sherri Goings, last updated 1/11/12 
   Created for CS201 (Data Structures) at Carleton College.
 *************************************************************************/


public class Tour {
    
    
    /* A helper class that defines a single node for use in a Tour.
       A node consists of a Point representing the location of that city in 
       the tour, and a pointer to the next Node in the tour. */
    private class Node {
	private Point p;
	private Node next;
        
        /* constructor creates a new Node at the given Point newp with an 
           initial next value of null. */
	public Node(Point newp) {
	    p = newp;
	    next = null;
	}
    }
    
    

    /* Use your main() function to test your code as you write it. This main() will not actually
       be ran once you have the entire Tour class complete, instead you will run the NearestInsertion
       and SmallestInsertion programs which call the functions in this class. */
    public static void main(String[] args) { 
        
        /* One example test could be:
           Tour tour = new Tour();
           Point p = new Point(0,0);
           tour.insertNearest(p);
           p = new Point(10,0);
           tour.insertNearest(p);
           p = new Point(10, 10);
           tour.insertNearest(p);
           System.out.println("Tour distance =  "+tour.distance());
           System.out.println("Number of points = "+tour.size());
           System.out.println(tour);

           // the tour size should be 3 and the distance 34.14 (don't forget to include the trip back
           // to the original point)
        */
        // uncomment the following section to draw the tour, setting w and h to the max x and y 
        // values that occur in your tour points
	/*
        int w =  ;
        int h =  ;
        StdDraw.setCanvasSize(w, h);
        StdDraw.setXscale(0, w);
        StdDraw.setYscale(0, h);
        StdDraw.setPenRadius(.005);
        tour.draw(); */
    }

}