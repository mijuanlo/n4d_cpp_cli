/*
* g++ -std=c++1z -o n4d_cli n4d_cli.cpp -lxmlrpc++ -lxmlrpc -lxmlrpc_xmlparse -lxmlrpc_xmltok -lxmlrpc_util -lxmlrpc_client++
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

using namespace std;

string toString(xmlrpc_c::value item){
    string c_string = "";

    switch(item.type()){
	case xmlrpc_c::value::TYPE_STRING:{
	    xmlrpc_c::value_string xml_string = xmlrpc_c::value_string(item);
	    //c_string = static_cast<string>(xml_string);
	    c_string = xml_string.crlfValue();
	    c_string = "'" + c_string + "'";
	    break;
	}
	case xmlrpc_c::value::TYPE_INT:{
	    xmlrpc_c::value_int xml_int = xmlrpc_c::value_int(item);
	    //int i = static_cast<int>(xml_int);
	    int i = xml_int.cvalue();
	    c_string = to_string(i);
	    break;
	}
	case xmlrpc_c::value::TYPE_I8:{
	    xmlrpc_c::value_i8 xml_long = xmlrpc_c::value_i8(item);
	    //long long i = static_cast<long long>(xml_long);
	    long long i = xml_long.cvalue();
	    c_string = to_string(i);
	    break;
	}
	case xmlrpc_c::value::TYPE_DOUBLE:{
	    xmlrpc_c::value_double xml_double = xmlrpc_c::value_double(item);
	    //double i = static_cast<double>(xml_double);
	    double i = xml_double.cvalue();
	    c_string = to_string(i);
	    break;
	}
	case xmlrpc_c::value::TYPE_BYTESTRING:{
	    xmlrpc_c::value_bytestring xml_bytestr = xmlrpc_c::value_bytestring(item);
	    //vector<unsigned char> vchar(xml_bytestr.vectorUcharValue());
	    vector<unsigned char> vchar = xml_bytestr.cvalue();
	    c_string = string(vchar.begin(),vchar.end());
	    break;
	}
	case xmlrpc_c::value::TYPE_BOOLEAN:{
	    xmlrpc_c::value_boolean xml_bool = xmlrpc_c::value_boolean(item);
	    //bool i = static_cast<bool>(xml_bool);
	    bool i = xml_bool.cvalue();
	    if (i){
		c_string = "True";
	    }else{
		c_string = "False";
	    }
	    break;
	}
	case xmlrpc_c::value::TYPE_NIL:{
	    c_string = "None";
	    break;
	}
	case xmlrpc_c::value::TYPE_DATETIME:{
	    xmlrpc_c::value_datetime xml_dt = xmlrpc_c::value_datetime(item);
	    time_t t = xml_dt.cvalue();
	    tm * ptm = localtime(&t);
	    char buf[32];
	    strftime(buf,32,"%a, %d.%m.%Y %H:%M:%S",ptm);
	    c_string = string(buf);
	    break;
	}
	case xmlrpc_c::value::TYPE_ARRAY:{
	    xmlrpc_c::value_array xml_array = xmlrpc_c::value_array(item);
	    vector<xmlrpc_c::value> c_vector = xml_array.vectorValueValue();
	    c_string = "[";
	    for (auto const& i : c_vector){
	    	    c_string += toString(i) + ",";
	    }
	    c_string.pop_back();
	    c_string += "]";
	    break;
	}
	case xmlrpc_c::value::TYPE_STRUCT:{
	    xmlrpc_c::value_struct xml_struct = xmlrpc_c::value_struct(item);
	    //map<string,xmlrpc_c::value> c_map = static_cast<map<string,xmlrpc_c::value>>(xml_struct);
	    map<string,xmlrpc_c::value> c_map = xml_struct.cvalue();
	    c_string = "{";
	    for (auto const& [key,val] : c_map){
		c_string += "'" + key + "':" + toString(val) + ",";
	    }
	    c_string.pop_back();
	    c_string += "}";
	    break;
	}
	default:{
	    cout << "ERROR UNKNOWN TYPE" << endl;
	    exit(1);	
	}
    }    
    return c_string;
}

void phelp(){
    cout << "-a: auto example getting auth function from localhost" << endl;
    cout << "-n: auto example getting anon function from localhost" << endl;
    cout << "-u: auth user" << endl;
    cout << "-p: auth password" << endl;
    cout << "-H: n4d host" << endl;
    cout << "-c: class name" << endl;
    cout << "-m: method name" << endl;
    cout << "-h: this help" << endl;
    cout << "-g: get methods call (same as: -H localhost -m get_methods) " << endl;
    cout << "-z: arguments" << endl;
    exit(0);
}
int main(int argc, char *argv[]) {

    string authUser = "";
    string authPwd = "";
    string n4dHost = "";
    string className = "";
    string methodName = "";
    bool AUTO_EXAMPLE = false;
    bool WITH_AUTH = false;
    bool is_param = false;
    vector<string> params;

    int i = 0;
    if (argc == 1){
	phelp();
    }
    while (i < argc){
	if (is_param and argv[i][0] != '-'){
	    // include parameter
	    params.push_back(argv[i++]);
	    continue;
	}
	if (argv[i][0] == '-'){
	    is_param = false;
	    switch(argv[i][1]){
		case 'u':
		    WITH_AUTH = true;
		    authUser = argv[++i];
		    break;
	        case 'p':
		    WITH_AUTH = true;
		    authPwd = argv[++i];
		    break;
		case 'H':
		    n4dHost = argv[++i];
		    break;
	        case 'c':
		    className = argv[++i];
		    break;
		case 'm':
		    methodName = argv[++i];
		    break;
		case 'a':
		    AUTO_EXAMPLE=true;
		    WITH_AUTH=true;
		    break;
		case 'n':
		    AUTO_EXAMPLE=true;
		    WITH_AUTH=false;
		    break;
		case 'h':
		    phelp();
		    break;
		case 'g':
		    methodName = "get_methods";
		    n4dHost = "localhost";
		    break;
		case 'z':
		    is_param = true;
		    break;
		default:
		    phelp();
		    break;
	    }
	}
	i++;
    }
    try {
	if (AUTO_EXAMPLE){
	    authUser = "netadmin";
	    authPwd = "lliurex";
            n4dHost = "https://localhost:9779/RPC2";
	    if (WITH_AUTH){
        	methodName = "get_available_groups";
	    }else{
		methodName = "get_teacher_list";
	    }
	    className = "Golem";
	}
	if (n4dHost == ""){
	    n4dHost="https://localhost:9779/RPC2";
	}
	size_t tst = n4dHost.find("http");
	if (tst  == string::npos ){
	    n4dHost="https://"+n4dHost+":9779/RPC2";
	}
	// cout << "Using " << n4dHost << endl;
	// Parametized options that allow n4dclient work with self-signed certificates
        xmlrpc_c::clientXmlTransport_curl myTransport
          (xmlrpc_c::clientXmlTransport_curl::constrOpt()
           .no_ssl_verifyhost(true)
           .no_ssl_verifypeer(true)
           );
        
	// Custom client with parametized options
        xmlrpc_c::client_xml myClient(&myTransport);

	// Example: Simple client that don't support self-signed certificates
        // xmlrpc_c::clientSimple myClient;
    
        xmlrpc_c::paramList callParams;
	// ((user,password)| ) classname? Parameters*

	// If need authentication
	if (WITH_AUTH){
	    if (authUser == "" or authPwd == ""){
		cout << "Need user & password" << endl;
		exit(1);
	    }
	    vector<xmlrpc_c::value> vParams;
	    vParams.push_back(xmlrpc_c::value_string(authUser));
    	    vParams.push_back(xmlrpc_c::value_string(authPwd));
	    xmlrpc_c::value_array aParams(vParams);
	    callParams.add(aParams);
	}else{ 	// If its anonymous call
	    callParams.add(xmlrpc_c::value_string(""));
	}
	if (className != ""){
	    callParams.add(xmlrpc_c::value_string(className));
	}
	if (params.size() != 0){
	    for (int i=0; i<params.size(); i++){
		callParams.add(xmlrpc_c::value_string(params[i]));
	    }
	}
        xmlrpc_c::rpcPtr myRpcP(methodName, callParams);
        
        xmlrpc_c::carriageParm_curl0 myCarriageParm(n4dHost);
        
        myRpcP->call(&myClient,&myCarriageParm);
        
	xmlrpc_c::value returned = myRpcP->getResult();
	
	// Example return if a simple call is used
        // string const res(xmlrpc_c::value_array(myRpcP->getResult()));

	cout << toString(returned) << endl;
	
	// Example simple call with two params
        //myClient.call(serverUrl, methodName, "ii", &result, 5, 7);
        
        //string const res = xmlrpc_c::value_string(result);
        // Assume the method returned an integer; throws error if not


    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    return 0;
}

