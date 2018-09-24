function saveToFile(txt, filename){
	var a = document.createElement('a');
	a.setAttribute("style", "display: none");
	document.body.appendChild(a);
	a.setAttribute('href', 'data:application/json;charset=utf-8,'+encodeURIComponent(txt));
	a.setAttribute('download', filename);
	a.click();
	document.body.removeChild(a);
}

function saveJSON(obj, filename){
	var txt = JSON.stringify(obj,"", "\t");
	saveToFile(txt, filename);
}

function showJSON(obj, filename) {
	var txt = JSON.stringify(obj,"", "\t");
	var a = window.open("", "_blank").document;
	a.write("<PLAINTEXT>");
	a.write(txt);
	a.title = filename;
}

function startsWith(str, prefix) {
	return str.substring(0, prefix.length) == prefix;
}

function endsWith(str, suffix) {
	return str.substring(str.length-suffix.length) == suffix;
}

function isDefined(obj) {
	return typeof obj !== 'undefined';
}

function isNumber(obj) {
	return isDefined(obj) && obj != null;
}

function isDefinedNN(obj) {
	return isDefined(obj) && obj != null;
}

function padLeft(str, chr, maxLen) {
	str = str.toString();
	return str.length < maxLen ? padLeft(chr + str, chr, maxLen) : str;
}

// sourceArray contains a list of objects that have a property "id". This methods makes a map using the "id" as a key, and the owning object as a value.
function copyArrayToMap(sourceArray, map) {
	if (!isDefined(map)) {
		map = {};
	}
	var len = sourceArray.length;
	for (var i = 0; i<len; i++) {
		var v = sourceArray[i];
		map[v.id] = v;
	}
	return map;
}

function checkTranslationFile(fileName) {
	return startsWith(fileName, "translation_") && endsWith(fileName, ".json") || confirm("Are you sure that you want to use "+fileName+" instead of a translation_*.json file?");
}

function xunescape(str) {
	return str.replace(/\\/g, "");
}
