<?php
/*
	https://github.com/RoboDurden/MicroControlLib
	2019/06/23 MicroControlLib v0.21 by Robo Durden 
	copyright: GNU General Public License v3.0 = GNU GPLv3
	
	bug fix 2019/07/17
			case "-":	$f = $o1->fRet - $o2->fRet;	break;
*/

class CMicroControlLib
{
	var $aValue; 	// the array of values
	var $aGlobal;	// the array of global variables
	var $aSystem;	// the array of system variables, 
					// 	some readonly like time, some writeonly like output bits
	var $sError;

	function __construct(&$aValue,&$aGlobal,&$aSystem)
	{
		$this->aValue = &$aValue;
		$this->aGlobal = &$aGlobal;
		$this->aSystem = &$aSystem;
	}

	function _Debug($s)
	{
		if (isset($_GET['debugMCL']))
			print str_replace(" ","&nbsp;",$s)."<br/>\n";
	}
	
	function _Error($sErr)
	{
		if ($this->sError)
			$this->sError .= '; '.$sErr;
		else 
			$this->sError = $sErr;
		return 0;
	}
	
	function &_GetTypeArray($sType)
	{
		switch($sType)
		{
		case "v":	return $this->aValue;
		case "g":	return $this->aGlobal;
		case "s":	return $this->aSystem;
		}
		return 0;	
	}
		
	function Calc($s)
	{
		$this->_Debug("Calc(): $s");
		$iLen = strlen($s);
		$o1 = $this->_NextNum($s,0,$iLen);
		while(1)
		{
			$sOp = $s[$o1->j];	// charAt
			$o2 = $this->_NextNum($s,$o1->j +1,$iLen);
			$f = 0;
			$this->_Debug($sOp.' o1:'.print_r($o1,true).'o2:'.print_r($o2,true));
			switch($sOp)
			{
			case ">":	$f = ($o1->fRet > $o2->fRet) ? 1 : 0;	break;
			case "<":	$f = ($o1->fRet < $o2->fRet) ? 1 : 0;	break;
			case "=":	$f = ($o1->fRet == $o2->fRet) ? 1 : 0;	break;
			case "+":	$f = $o1->fRet + $o2->fRet;	break;
			case "-":	$f = $o1->fRet - $o2->fRet;	break;
			case "*":	$f = $o1->fRet * $o2->fRet;	break;
			case "/":	
				if (!$o2->fRet)
					return $this->_Error('division by zero');
				$f = $o1->fRet / $o2->fRet;	
				break;
			case "|":	$f = $o1->fRet | $o2->fRet;	break;
			case "&":	$f = $o1->fRet & $o2->fRet;	break;
			case "~":	
				if (!$o2->t)	// syntax error: can not set o1->f to a constant variable
	        		return $this->_Error("$s : can not set '$o1->fRet' to constant.");
				$a = &$this->_GetTypeArray($o2->t);
				$a[$o2->f] = $f = $o1->fRet;
				$this->_Debug("  -> ".print_r($this->aGlobal,true));
				break;
			default:	// unkown operator
	        	return $this->_Error("$s : unkown operator '$sOp'");
			}
			if ($o2->j >= $iLen)
				break;
			$o1 = (object)["t" => 0, "i" => $o2->i, "j" => $o2->j, "fRet" => $f];
		}
		return $f;
	}

	function _NextNum($s,$i,$j)
	{
		$this->_Debug(" _NextNum($s,$i,$j)");
		//return 0;
		while($i<$j)
		{
			$c = $s[$i++];	// charAt
			//$this->_Debug("$i,$j -> '$c'");
			if ($c == "(")
			{
				$iOpen = 1;
				$j2=$i;
				while($j2<$j)
				{
					$c = $s[$j2++];	// charAt
					if ($c == "(")
					{
						$iOpen++;
					}	
					if ($c == ")")
					{
						$iOpen--;
						break;
					}	
				}
				if ($iOpen>0)	
					return $this->_Error("$s : syntax error too many '('");
				return (object)["t" => 0,"f" => 0,"i" => $i,"j" => $j2,"fRet" => $this->Calc(substr($s,$i,$j2-$i-1))];
			}
			$cType = 0;	// expecting this to be a constant number
			$j2=--$i;
			while($j2<$j)
			{
				switch($c)
				{
				case "+":
				case "-":
					if ($j2>$i)	// we already parsed a number
						if ($cType)	// we already parsed a param
							break 2;
						else
							return (object)["t" => 0,"f" => 0,"i" => $i,"j" => $j2,"fRet" => substr($s,$i,$j2-$i)];
				case ".":
					if ($cType)	// syntax error
						return $this->_Error("$s : syntax error #2");
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
					if ($cType)
					{
						$iPos = substr($s,$i,$j2-$i);
						$a = $this->_GetTypeArray($cType);
						$f=$a[$iPos];
						$this->_Debug("  get1 $cType$iPos = '$f'");
						return (object)["t" => $cType,"f" => $iPos,"i" => $i,"j" => $j2,"fRet" => $f];
					}
					if ($j2>$i)	// we already parsed a number
						return (object)["t" => 0,"f" => 0,"i" => $i,"j" => $j2,"fRet" => 1*substr($s,$i,$j2-$i)];
					$cType = $c;	// seems to be something linke "v1"
					$i++;
				}
				$c = $s[++$j2];	// charAt
			}
			if ($cType)
			{
				$iPos = substr($s,$i,$j2-$i);
				$a = $this->_GetTypeArray($cType);
				$f=$a[$iPos];
				$this->_Debug("  get2 $cType$iPos = '$f'");
				return (object)["t" => $cType,"f" => $iPos,"i" => $i,"j" => $j2,"fRet" => $f];
			}
			return (object)["t" => $cType,"f" => 0,"i" => $i,"j" => $j2,"fRet" => 1*substr($s,$i,$j2-$i)];
		}
	}
}
?>
