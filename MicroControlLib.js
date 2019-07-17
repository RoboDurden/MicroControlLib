/*
	https://github.com/RoboDurden/MicroControlLib
	2019/06/21 MicroControlLib v0.21 by Robo Durden 
	copyright: GNU General Public License v3.0 = GNU GPLv3
	bug fix 2019/07/17
			case "-":	f = o1.fRet - o2.fRet;	break;
	bug fix 2019/06/21:
fertig:
		while(j2<j)
...	
			case "-":
				if (j2>i)	// we already parsed a number
					if (cType)	// we already parsed a param
						break fertig;
					else
						return {"t":0,"f":0,"i":i,"j":j2,"fRet":s.substr(i,j2-i)};
	
*/
function CMicroControlLib(aValue,aGlobal,aSystem)
{
	this.aValue = aValue; 	// the array of values
	this.aGlobal = aGlobal;	// the array of global variables
	this.aSystem = aSystem;	// the array of system variables, 
					// 	some readonly like time, some writeonly like output bits
	this.sError = "";
	
	this._Error = function _Error(sErr)
	{with(this){
		if (sError)
			sError += '; '+sErr;
		else 
			sError = sErr;
		return 0;
	}}

	this.Init = function Init(aValue,aGlobal,aSystem)
	{
		this.aValue = aValue;
		this.aGlobal = aGlobal;
		this.aSystem = aSystem;
	}
	
	this._GetTypeArray = function _GetTypeArray(sType)
	{with(this){
		switch(sType)
		{
		case "v":	return aValue;
		case "g":	return aGlobal;
		case "s":	return aSystem;
		}
		return 0;	
	}}
		
	this.Calc = function Calc(s)
	{with(this){
		var iLen = s.length;
		var o1 = _NextNum(s,0,iLen);
		while(1)
		{
			var sOp = s.charAt(o1.j);
			var o2 = _NextNum(s,o1.j+1,iLen);
			var f = 0;
			switch(sOp)
			{
			case ">":	f = (o1.fRet > o2.fRet) ? 1 : 0;	break;
			case "<":	f = (o1.fRet < o2.fRet) ? 1 : 0;	break;
			case "=":	f = (o1.fRet == o2.fRet) ? 1 : 0;	break;
			case "+":	f = o1.fRet + o2.fRet;	break;
			case "-":	f = o1.fRet - o2.fRet;	break;
			case "*":	f = o1.fRet * o2.fRet;	break;
			case "/":
					if (!o2.fRet)	return _Error('division by zero');
					f = o1.fRet / o2.fRet;	break;
			case "|":	f = o1.fRet | o2.fRet;	break;
			case "&":	f = o1.fRet & o2.fRet;	break;
			case "~":	
				if (!o2.t)	// syntax error: can not set o1.f to a constant variable
	        		return _Error(s + " : can not set "+ o1.fRet +" to constant.");
				var a = _GetTypeArray(o2.t);
				a[o2.f] = f = o1.fRet;
				break;
			default:	// unkown operator
	        	return _Error(s + " : unkown operator "+ sOp);
			}
			if (o2.j >= iLen)
				break;
			o1 = {"t":0,"i":o2.i,"j":o2.j,"fRet":f};
		}
		return f;
	}}

	this._NextNum = function _NextNum(s,i,j)
	{with(this){
		//return 0;
		while(i<j)
		{
			var c = s.charAt(i++);
			if (c == "(")
			{
				var iOpen = 1;
				var j2=i;
				while(j2<j)
				{
					var c = s.charAt(j2++);
					if (c == "(")
					{
						iOpen++;
					}	
					if (c == ")")
					{
						iOpen--;
						break;
					}	
				}
				if (iOpen>0)
					return _Error(s + " : syntax error, too many '('");
				return {"t":0,"f":0,"i":i,"j":j2,"fRet":Calc(s.substr(i,j2-i-1))};
			}
			var cType = 0;	// expecting this to be a constant number
			var j2=--i;
	fertig:
			while(j2<j)
			{
				switch(c)
				{
				case "+":
				case "-":
					if (j2>i)	// we already parsed a number
						if (cType)	// we already parsed a param
							break fertig;
						else
							return {"t":0,"f":0,"i":i,"j":j2,"fRet":s.substr(i,j2-i)};
				case ".":
					if (cType)	// syntax error
						return _Error(s + " : syntax error #2");
				case "0":
				case "1":
				case "2":
				case "3":
				case "4":
				case "5":
				case "6":
				case "7":
				case "8":
				case "9":
					break;
				default: 
					if (cType)
					{
						var iPos = s.substr(i,j2-i);
						var a = _GetTypeArray(cType);
						var f=a[iPos];
						return {"t":cType,"f":iPos,"i":i,"j":j2,"fRet":f};
					}
					if (j2>i)	// we already parsed a number
						return {"t":0,"f":0,"i":i,"j":j2,"fRet":1*s.substr(i,j2-i)};
					cType = c;	// seems to be something linke "v1"
					i++;
				}
				c = s.charAt(++j2);
			}
			if (cType)
			{
				var iPos = s.substr(i,j2-i);
				var a = _GetTypeArray(cType);
				var f=a[iPos];
				return {"t":cType,"f":iPos,"i":i,"j":j2,"fRet":f};			
			}
			return {"t":cType,"f":0,"i":i,"j":j2,"fRet":1*s.substr(i,j2-i)};
		}
	}}
}
