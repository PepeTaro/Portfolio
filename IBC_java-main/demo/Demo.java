import java.util.List;
import java.math.BigInteger;


public class Demo{    
    
    public static void main(String[] args){	
	int securityBit = 20;
	String plainText = "111111111111111111111111111111";
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

	System.out.printf("平文:   %s\n",plainText);
	System.out.printf("復号文: %s\n",decText);
	
	System.out.printf("復号化に成功? %b\n",plainText.equals(decText));
    }
}
