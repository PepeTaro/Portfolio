public class Test{            
    public static void assertTrue(boolean test,String message){
	if(! test)
	    throw new RuntimeException(message);
    }
}
