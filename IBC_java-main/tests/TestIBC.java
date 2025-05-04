import java.util.Arrays;
import java.util.List;
import java.math.BigInteger;

public class TestIBC{

    private static void testXor(){
	String error = "Error: TestIBC.testXor()";
	String s1,s2,s;
        s1 = "0101010011";
	s2 = "1111111111";
	s  = "1010101100";
	Test.assertTrue(IBC.xor(s1,s2).equals(s),error);

	s1 = "000000000000000000000000";
	s2 = "111111111111111111111111";
	s  = "111111111111111111111111";
	Test.assertTrue(IBC.xor(s1,s2).equals(s),error);

	s1 = "111111111111111111111111";
	s2 = "111111111111111111111111";
	s  = "000000000000000000000000";
	Test.assertTrue(IBC.xor(s1,s2).equals(s),error);

	s1 = "000000000000000000000000";
	s2 = "000000000000000000000000";
	s  = "000000000000000000000000";
	Test.assertTrue(IBC.xor(s1,s2).equals(s),error);

	s1 = "101010101010101010101010";
	s2 = "010101010101010101010101";
	s  = "111111111111111111111111";
	Test.assertTrue(IBC.xor(s1,s2).equals(s),error);
    }
    
    private static void testEncAndDec(String plainText){

	int securityBit = 20;
	//String plainText = "111111111111111111111111111111";
	int n = plainText.length();
	
	List<Object> ecParams = IBC.generateEC(securityBit);
	EC<Fq2> ec = (EC<Fq2>)ecParams.get(0);
	Point<Fq2> p = (Point<Fq2>)ecParams.get(1);
	BigInteger order = (BigInteger)ecParams.get(2);

	List<Object> pubkeyParams = IBC.generatePublicKeyParams(ec,p,order);
	BigInteger s = (BigInteger)pubkeyParams.get(0);
	Point<Fq2> pubkey = (Point<Fq2>)pubkeyParams.get(1);

	String id = "Alice@gmail.com";
	List<Object> privkeyParams = IBC.generatePrivateKeyParams(ec,s,id);
	Point<Fq2> h1 = (Point<Fq2>)privkeyParams.get(0);
	Point<Fq2> privkey = (Point<Fq2>)privkeyParams.get(1);

	List<Object> encData = IBC.encrypt(ec,plainText,pubkey,h1,p,order,n);
	Point<Fq2> c1 = (Point<Fq2>)encData.get(0);
	String c2 = (String)encData.get(1);
	
        String decText = IBC.decrypt(ec,c1,c2,privkey,order,n);

	Test.assertTrue(plainText.equals(decText),"Error: TestIBC.testEncAndDec()");	
    }

    private static void testMultipleEncAndDec(){
	String plainText;
	for(int i=0;i< (1 << 10);i++){
	    plainText = Integer.toBinaryString(i);
	    testEncAndDec(plainText);	    
	}

	String longText = "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
	
	String[] plainTexts = {
	    "",
	    "0",
	    "1",
	    longText
	};

	for(String s: plainTexts){
	    testEncAndDec(s);
	}
    }
    
    public static void main(String[] args){
	testXor();
	testMultipleEncAndDec();
    }
}
