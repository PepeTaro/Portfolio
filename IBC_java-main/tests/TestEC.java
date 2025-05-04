import java.math.BigInteger;

public class TestEC{
    
    private static void testECOverFq(){
	String error = "Error: TestEC.testECOverFq()";
	
	EC<Fq> ec;
	Fq a,b;
	Point<Fq> p,q,r,expected;
	Point<Fq> INF;
	
	Fq.setPrime("13");
        a = new Fq("3");
	b = new Fq("8");
	ec = new EC<Fq>(a,b);
	INF = ec.INF();
	
        p = new Point<Fq>(new Fq("9"),new Fq("7"));
        q = new Point<Fq>(new Fq("1"),new Fq("8"));
	
	// p + q
	expected = new Point<Fq>(new Fq("2"),new Fq("10"));	
        Test.assertTrue(ec.add(p,q).equals(expected),error);

	// p + p
	expected = new Point<Fq>(new Fq("9"),new Fq("6"));
        Test.assertTrue(ec.add(p,p).equals(expected),error);

	// INF + INF = INF
	expected = new Point<Fq>(new Fq("9"),new Fq("6"));
        Test.assertTrue(ec.add(INF,INF).equals(INF),error);

	// p + INF = p
        Test.assertTrue(ec.add(p,INF).equals(p),error);

	// INF + q = q
        Test.assertTrue(ec.add(INF,q).equals(q),error);

	Fq.setPrime("73");
        a = new Fq("8");
	b = new Fq("7");
	ec = new EC<Fq>(a,b);
	p = new Point<Fq>(new Fq("32"),new Fq("53"));
	q = ec.multiply(new BigInteger("11"),p);
	
	expected = new Point<Fq>(new Fq("39"),new Fq("17"));	
        Test.assertTrue(q.equals(expected),error);


	Fq.setPrime("97");
        a = new Fq("2");
	b = new Fq("3");
	ec = new EC<Fq>(a,b);
	p = new Point<Fq>(new Fq("3"),new Fq("6"));
	
	Point[] expectedVals = {
	    INF,
	    p,
	    new Point<Fq>(new Fq("80"),new Fq("10")),
	    new Point<Fq>(new Fq("80"),new Fq("87")),
	    new Point<Fq>(new Fq("3"),new Fq("91")),
	    INF,
	    p
	};

	r = INF;
	for(int i=0;i<expectedVals.length;i++){
	    q = ec.multiply(BigInteger.valueOf(i),p);
	    Test.assertTrue(q.equals(expectedVals[i]),error);
	    Test.assertTrue(r.equals(expectedVals[i]),error);
	    
	    r = ec.add(r,p);
	}
	
    }

    private static void testECOverFq2(){
	String error = "Error: TestEC.testECOverFq2()";
	
	Fq2.setPrime("5");
	Fq2 a = new Fq2("1","2");
	Fq2 b = new Fq2("3","4");
	EC<Fq2> ec = new EC<Fq2>(a,b);

	Point<Fq2> p,q,r;
	p = new Point<Fq2>(new Fq2("1","4"),new Fq2("2","3"));
	q = ec.INF();
	for(int i=1;i<30;i++){
	    q = ec.add(p,q);
	    r = ec.multiply(i,p);
	    Test.assertTrue(q.equals(r),error);
	}
	
    }

    public static void main(String[] args){
	testECOverFq();
	testECOverFq2();
    }
}
