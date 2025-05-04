import java.math.BigInteger;

public class TestFq2{    
    //private static final int[] primes = {2,5,11,17,23,29,41,47,53,59,71,83,89,101};
    private static final int[] primes = {2,11,23};
    
    private static void testArithmetic(){
	for(int prime : primes){
	    Fq2.setPrime(BigInteger.valueOf(prime));
	    
	    for(int i=0;i<prime;i++){
		for(int j=0;j<prime;j++){
		    testSquare(prime,i,j);		    
		    testInverse(prime,i,j);
		    testNegate(prime,i,j);
		    testTrace(prime,i,j);
		    
		    for(int k=0;k<prime;k++){
			testPow(prime,i,j,k);
			
			for(int l=0;l<prime;l++){
			    testAdd(prime,i,j,k,l);			    
			    testSubtract(prime,i,j,k,l);
			    testMultiply(prime,i,j,k,l);
			    testDivide(prime,i,j,k,l);			    			    
			}
		    }
		}
	    }
	}
    }
    
    private static void testAdd(int prime,int i,int j,int k,int l){
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
	Fq2 b = new Fq2(BigInteger.valueOf(k),BigInteger.valueOf(l));
	Fq2 resultVal   = a.add(b);	
	Fq2 expectedVal = new Fq2(BigInteger.valueOf((i + k)%prime),BigInteger.valueOf((j + l)%prime));

	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq2.testAdd()");
    }
    
    private static void testSubtract(int prime,int i,int j,int k,int l){
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
	Fq2 b = new Fq2(BigInteger.valueOf(k),BigInteger.valueOf(l));
	Fq2 resultVal   = a.subtract(b);	
	Fq2 expectedVal = new Fq2(BigInteger.valueOf((i - k)%prime),BigInteger.valueOf((j - l)%prime));

	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq2.testSubtract()");
    }

    private static void testMultiply(int prime,int i,int j,int k,int l){
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
	Fq2 b = new Fq2(BigInteger.valueOf(k),BigInteger.valueOf(l));
	Fq2 resultVal   = a.multiply(b);
	
	long x = (j*l - i*l - j*k)%prime;
	long y = (i*k - i*l - j*k)%prime;
        Fq2 expectedVal = new Fq2(BigInteger.valueOf(x),BigInteger.valueOf(y));
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq2.testMutiply()");
    }
    
    private static void testDivide(int prime,int i,int j,int k,int l){
	if(k == 0 && l == 0) return;

	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
	Fq2 b = new Fq2(BigInteger.valueOf(k),BigInteger.valueOf(l));
	Fq2 resultVal1   = a.divide(b); // a/b
	Fq2 resultVal2   = b.multiply(resultVal1); // b*(a/b) == a
        Fq2 expectedVal  = a;

	Test.assertTrue(resultVal2.equals(expectedVal),"Error: TestFq2.testDivide()");
    }

    private static void testSquare(int prime,int i,int j){
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
        Fq2 resultVal   = a.square();

        long x = (j*(j - 2*i))%prime;
	long y = (i*(i - 2*j))%prime;
        Fq2 expectedVal = new Fq2(BigInteger.valueOf(x),BigInteger.valueOf(y));
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq2.testSquare()");
    }
    
    private static void testPow(int prime,int i,int j,int k){
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
        Fq2 resultVal = a.pow(BigInteger.valueOf(k));

	// (long) Math.pow(i,j)とするとオーバーフローするため、for-loopを使用。
	// [注意] Fq2の単位元は(-1,-1)
	long x = -1;
	long y = -1;
	long tmp_x,tmp_y;
	
	for(int l=0;l<k;l++){
	    tmp_x = (j*y - i*y - j*x)%prime;
	    tmp_y = (i*x - i*y - j*x)%prime;
	    x = tmp_x;
	    y = tmp_y;
	}
	
        Fq2 expectedVal = new Fq2(BigInteger.valueOf(x),BigInteger.valueOf(y));
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq2.testPow()");
    }

    private static void testInverse(int prime,int i,int j){
	if(i == 0 && j == 0) return;
	
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
	Fq2 resultVal1   = a.inverse(); // 1/a
	Fq2 resultVal2   = a.multiply(resultVal1); // a*(1/a) == 1
	    
	Test.assertTrue(resultVal2.isOne(),"Error: TestFq2.testInverse()");
    }
    
    private static void testNegate(int prime,int i,int j){	
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
	Fq2 resultVal   = a.negate();
	Fq2 expectedVal = new Fq2(BigInteger.valueOf((-i)%prime),BigInteger.valueOf((-j)%prime));
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq2.testNegate()");
    }
    
    private static void testTrace(int prime,int i,int j){
	Fq2 a = new Fq2(BigInteger.valueOf(i),BigInteger.valueOf(j));
        Fq resultVal   = a.trace();
        Fq expectedVal = (new Fq(BigInteger.valueOf((i + j)%prime))).negate();
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq2.testTrace()");
    }
    
    public static void main(String[] args) {
	testArithmetic();
    }
}
