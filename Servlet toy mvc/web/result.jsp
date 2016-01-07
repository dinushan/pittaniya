<%@ page import"java.util.*" %>

<html>
<body>
<h1 align="center">Response JSP</h1>
<p>

<%
	List styles = (List)request.getAttribute("array");
	Iterator it = styles.iterator();
	while (it.hasNext()){
		out.println("<br>"+ it.next());
	}
%>
</body>
</html>