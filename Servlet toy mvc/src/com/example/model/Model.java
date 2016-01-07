package com.example.model;

import java.util.*;

public class Model{
	public List getLanguageList(String domain){
		List langs = new ArrayList();
		if (domain.equals("HDL")){
			langs.add("Verilog");
			langs.add("VHDL");
		}
		else{
			langs.add("C++");
			langs.add("Java");
		}
		return (langs);
	}
}

