import java.math.BigInteger;

public class TestPairing{
    
    public static void main(String[] args){	
	BigInteger m = new BigInteger("26107");
	Fq2.setPrime(new BigInteger("156641"));
	EC<Fq2> ec = new EC<Fq2>(new Fq2("0"),new Fq2("1"));

	Point<Fq2> p = new Point<Fq2>(new Fq2("79848"),new Fq2("30485"));
	Point<Fq2> q   = new Point<Fq2>(new Fq2("108737"),new Fq2("80338"));
	Point<Fq2> s   = new Point<Fq2>(new Fq2("73465"),new Fq2("10843"));
	
	Point<Fq2> qs = ec.add(q,s);        // ((154051,154051),(58098,58098)) 
	Point<Fq2> ps = ec.subtract(p,s);   // ((57398,57398),(101754,101754)) 
	Point<Fq2> negS = ec.negate(s);     // ((83176,83176),(10843,10843)) 
	
	Fq2 fpqs = Pairing.miller(ec,p,m,qs); // (26296,26296) 
	Fq2 fps  = Pairing.miller(ec,p,m,s);  // (33658,33658)
	
	Fq2 fqps   = Pairing.miller(ec,q,m,ps); // (80888,80888)
	Fq2 fqNegs = Pairing.miller(ec,q,m,negS); // (38631,38631) 
	
	Fq2 numer = fpqs.divide(fps);     // (40181,40181) 
	Fq2 denom = fqps.divide(fqNegs);  // (7848,7848) 
	Fq2 result = numer.divide(denom); // (91269,91269) 

	String error = "Error: TestPairing";
	Point<Fq2> qsExp = new Point<Fq2>(new Fq2("2590"),new Fq2("98543"));	
	Test.assertTrue(qs.equals(qsExp),error);

	Point<Fq2> psExp = new Point<Fq2>(new Fq2("99243"),new Fq2("54887"));	
	Test.assertTrue(ps.equals(psExp),error);

	Point<Fq2> negSExp = new Point<Fq2>(new Fq2("73465"),new Fq2("145798"));	
	Test.assertTrue(negS.equals(negSExp),error);

        Fq2 fpqsExp = new Fq2("130345");
	Test.assertTrue(fpqs.equals(fpqsExp),error);

	Fq2 fpsExp = new Fq2("122983");
	Test.assertTrue(fps.equals(fpsExp),error);

	Fq2 fqpsExp = new Fq2("75753");
	Test.assertTrue(fqps.equals(fqpsExp),error);
	
	Fq2 fqNegsExp = new Fq2("118010");
	Test.assertTrue(fqNegs.equals(fqNegsExp),error);

	Fq2 numerExp = new Fq2("116460");
	Test.assertTrue(numer.equals(numerExp),error);
	
	Fq2 denomExp = new Fq2("148793");
	Test.assertTrue(denom.equals(denomExp),error);

	Fq2 resultExp = new Fq2("65372");
	Test.assertTrue(result.equals(resultExp),error);

    } 
}
