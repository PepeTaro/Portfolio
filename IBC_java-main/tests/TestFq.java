import java.math.BigInteger;

public class TestFq{    
    private static final int[] primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103};
    
    private static void testArithmetic(){
	for(int prime : primes){
	    Fq.setPrime(BigInteger.valueOf(prime));
	    
	    for(int i=0;i<prime;i++){
		testSquare(prime,i);
		testInverse(prime,i);
		testNegate(prime,i);
		testTrace(prime,i);
		
		for(int j=0;j<prime;j++){
		    testAdd(prime,i,j);
		    testSubtract(prime,i,j);
		    testMultiply(prime,i,j);
		    testDivide(prime,i,j);
		    testPow(prime,i,j);		    
		}
	    }
	}
    }
    
    private static void testAdd(int prime,int i,int j){		    
        Fq a = new Fq(BigInteger.valueOf(i));
        Fq b = new Fq(BigInteger.valueOf(j));
        Fq resultVal = a.add(b);
	BigInteger expectedVal = BigInteger.valueOf((i + j)%prime);

	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq.testAdd()");
    }

    private static void testSubtract(int prime,int i,int j){		    
        Fq a = new Fq(BigInteger.valueOf(i));
        Fq b = new Fq(BigInteger.valueOf(j));
        Fq resultVal = a.subtract(b);
	BigInteger expectedVal = BigInteger.valueOf((i - j)%prime);

	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq.testSubtract()");
    }

    private static void testMultiply(int prime,int i,int j){		    
        Fq a = new Fq(BigInteger.valueOf(i));
        Fq b = new Fq(BigInteger.valueOf(j));
        Fq resultVal = a.multiply(b);
	BigInteger expectedVal = BigInteger.valueOf((i*j)%prime);
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq.testMultiply()");
    }

    
    private static void testDivide(int prime,int i,int j){
	if(j%prime == 0) return;

        Fq a = new Fq(BigInteger.valueOf(i));
        Fq b = new Fq(BigInteger.valueOf(j));
        Fq resultVal1 = a.divide(b);
	Fq resultVal2 = b.multiply(resultVal1);
	BigInteger expectedVal = BigInteger.valueOf(i);

	Test.assertTrue(resultVal2.equals(expectedVal),"Error: TestFq.testDivide()");
    }

    private static void testPow(int prime,int i,int j){
	Fq a = new Fq(BigInteger.valueOf(i));
        Fq resultVal = a.pow(BigInteger.valueOf(j));
	
	// (long) Math.pow(i,j)とするとオーバーフローするため、for-loopを使用。
	long val = 1;
	for(int k=0;k<j;k++)
	    val = (val*i)%prime;	
	BigInteger expectedVal = BigInteger.valueOf(val);
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq.testPow()");
    }
        
    private static void testSquare(int prime,int i){		    
        Fq a = new Fq(BigInteger.valueOf(i));
        Fq resultVal = a.square();
	BigInteger expectedVal = BigInteger.valueOf((i*i)%prime);

	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq.testSquare()");
    }

    private static void testInverse(int prime,int i){
	if(i%prime == 0) return;
	
	Fq a = new Fq(BigInteger.valueOf(i));
        Fq resultVal1   = a.inverse(); // 1/a
        Fq resultVal2   = a.multiply(resultVal1); // a*(1/a) == 1
	    
	Test.assertTrue(resultVal2.equals(BigInteger.ONE),"Error: TestFq.testInverse()");
    }

    private static void testNegate(int prime,int i){
	Fq a = new Fq(BigInteger.valueOf(i));
        Fq resultVal   = a.negate();
        Fq expectedVal = new Fq(BigInteger.valueOf((-i)%prime));
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq.testNegate()");
    }

    private static void testTrace(int prime,int i){
	Fq a = new Fq(BigInteger.valueOf(i));
        Fq resultVal   = a.trace();
        Fq expectedVal = new Fq(BigInteger.valueOf(i%prime));
	
	Test.assertTrue(resultVal.equals(expectedVal),"Error: TestFq.testTrace()");
    }
    
    public static void main(String[] args) {
	testArithmetic();
    }
}
