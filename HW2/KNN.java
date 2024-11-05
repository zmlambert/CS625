package HW2;

import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.awt.Graphics;
import java.awt.Color;
import java.io.IOException;
import java.io.File;
import java.util.Random;

class KNN extends JPanel {
    public int iterations = 10_000_000; // adjust as needed
    public double lambda = 0.1;
    public static int SIZE = 600; // please dont change :(
    private int repaintEvery = 1;

    private double[][] k; // weights

    public static void main(String[] args) {
        KNN k = new KNN();
        JFrame f = new JFrame();
        f.add(k);
        // magic numbers I found to get my window to display all points
        f.setSize(SIZE + 4, SIZE + 24);
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        f.setVisible(true);
        k.trainKohonen();
    }

    /*
     * Save image of current state of graphics
     */
    public void saveStateAsImage(String filename) {
        try {
            BufferedImage image = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_RGB);
            Graphics gfx = image.createGraphics();
            printAll(gfx);
            gfx.dispose();
            ImageIO.write(image, "png", new File(filename + ".png"));
            System.out.println("Saved " + filename + ".png");
        } catch (IOException e) {
            System.out.println("Failed to save " + filename + ".png: IOException");
        }
    }

    /*
     * Randomizer function for input data
     */
    public double[] spawnDataPointInRegion() {
        Random r = new Random();
        double[] x = new double[2];
        int region = r.nextInt(6);
        switch (region) {
            case 0: // TOP LEFT
                x[0] = r.nextDouble() * 200;
                x[1] = r.nextDouble() * 200;
                break;
            case 1: // MIDDLE LEFT
                x[0] = r.nextDouble() * 200;
                x[1] = r.nextDouble() * 200 + 200;
                break;
            case 2: // BOTTOM LEFT
                x[0] = r.nextDouble() * 200;
                x[1] = r.nextDouble() * 200 + 400;
                break;
            case 3: // MIDDLE
                x[0] = r.nextDouble() * 200 + 200;
                x[1] = r.nextDouble() * 200 + 200;
                break;
            case 4: // TOP RIGHT
                x[0] = r.nextDouble() * 200 + 400;
                x[1] = r.nextDouble() * 200;
                break;
            case 5: // BOTTOM RIGHT
                x[0] = r.nextDouble() * 200 + 400;
                x[1] = r.nextDouble() * 200 + 400;
                break;
            default:
                // if this happens..... Java changed the implementation of nextInt.
                // or maybe a solar flare flipped a bit on my hard drive.
                throw new IllegalStateException("Unexpected region value: " + region);
        }
        return x;
    }

    /*
     * Initializes Kohonen neural network weight matrix given sizes m and n
     */
    public double[][] initWeights(int m, int n) {
        double[][] k = new double[m][n];
        Random r = new Random();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                k[i][j] = r.nextDouble() * SIZE;
            }
        }
        return k;
    }

    /*
     * Paint method to display the Kohonen neurons
     */
    protected void paintComponent(Graphics g) {
        super.paintComponent(g); // clear
        drawKohonen(g); // draw
    }

    /*
     * Draw the Kohonen neurons to the graphics context
     * mostly taken from slides.
     */
    public void drawKohonen(Graphics g) {
        // BLANK OUT CANVAS
        g.setColor(Color.white);
        g.fillRect(0, 0, this.getWidth(), this.getHeight());

        // DRAW REGIONS
        g.setColor(Color.red); // TOP LEFT
        g.fillRect(0,0,200,200);
        g.setColor(Color.orange);
        g.fillRect(0, 200, 200, 200); // MIDDLE LEFT
        g.setColor(Color.yellow);
        g.fillRect(0, 400, 200, 200); // BOTTOM LEFT
        g.setColor(Color.green);
        g.fillRect(200, 200, 200, 200); // MIDDLE
        g.setColor(Color.blue);
        g.fillRect(400, 0, 200, 200); // TOP RIGHT
        g.setColor(Color.pink);
        g.fillRect(400, 400, 200, 200); // BOTTOM RIGHT

        // DRAW KOHONEN NEURONS
        g.setColor(Color.black);
        for (int i = 0; i < k.length; i++)
            g.drawString("K", (int) k[i][0], (int) k[i][1]);
    }

    /*
     * Training method for Kohonen mostly taken from slides
     */
    public void trainKohonen() {
        k = initWeights(500, 2); // init weights
        double lambda = this.lambda;
        int iterations = this.iterations;
        double deltaLambda = lambda / iterations;

        // save initial state to image
        saveStateAsImage("iter_0");
        for (int iter = 0; lambda > 0; iter++) {
            double[] x = spawnDataPointInRegion();
            int winner = 0;
            double dist = 0;

            // find winning neuron
            for(int j = 0; j < k[0].length; j++) {
                dist += Math.pow(k[0][j] - x[j], 2);
            }
            for (int i = 0; i < k.length; i++) {
                double iDist = 0;
                // distance calculation for current neuron
                for (int j = 0; j < k[i].length; j++) {
                    iDist += Math.pow(k[i][j] - x[j], 2);
                }
                // check if current neuron is winning neuron
                if (iDist < dist) {
                    winner = i;
                    dist = iDist;
                }
            }
            
            // move winning neuron toward data point
            for (int j = 0; j < k[winner].length; j++) {
                k[winner][j] += lambda * (x[j] - k[winner][j]);
            }
            
            lambda -= deltaLambda;
            
            // save images for multiples of 10 iterations
            if(iter == 11) saveStateAsImage("iter_10");
            if(iter == 101) saveStateAsImage("iter_100");
            if(iter == 1001) saveStateAsImage("iter_1000");
            if(iter == 10001) saveStateAsImage("iter_10000");
            if(iter == 100001) saveStateAsImage("iter_100000");
            if(iter == 1_000_001) saveStateAsImage("iter_1000000");

            // redraw gui every "repaintEvery" iterations
            if (iter % this.repaintEvery == 0) repaint();
        }
        System.out.println("done");
        // save image of final state
        saveStateAsImage("iter_final");
    }
}
