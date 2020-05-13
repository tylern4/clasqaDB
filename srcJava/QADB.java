import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;
import java.io.*;

public class QADB {

  public Object obj;
  public JSONObject jsonObj;

  public QADB(String jsonFileName) {
    try {
      obj = new JSONParser().parse(new FileReader(jsonFileName));
      jsonObj = (JSONObject) obj;
    } 
    catch (FileNotFoundException e) { e.printStackTrace(); }
    catch (IOException e) { e.printStackTrace(); }
    catch (ParseException e) { e.printStackTrace(); }
  }

  public boolean Query(int runnum_, int filenum_) {
    Map runTree = (Map) jsonObj.get(String.valueOf(runnum_));
    Map fileTree = (Map) runTree.get(String.valueOf(filenum_));
    System.out.println("defect = "+fileTree.get("defect"));
    return true;
  }


};

