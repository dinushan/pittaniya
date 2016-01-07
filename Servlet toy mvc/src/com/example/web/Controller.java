package com.example.web;

import com.example.model.Model;
import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;

public class Controller extends HttpServlet{
	public void doPost(HttpServletRequest request,
			HttpServletResponse response)
			throws IOException, ServletException{

		String c = request.getParameter("domain");
		Model model = new Model();
		List result = model.getLanguageList(c);

		request.setAttribute("array",result);
		RequestDispatcher view = request.getRequestDispatcher("result.jsp");
		view.forward(request,response);
	}
}
