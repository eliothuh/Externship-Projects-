function toggleDropdownStatesVisibility() {
  var x = document.getElementById("dropDownStates");
  var y = document.getElementById("dropDownYears");
  if (x.style.display === "none") {
	x.style.display = "flex";
	y.style.display = "none";
  } else {
	x.style.display = "none";
  }
}

function turnOffAllCollapsibles() {
	var y = document.getElementById("dropDownStates");
	var x = document.getElementById("dropDownYears");
	x.style.display="none";
	y.style.display="none";
}

function toggleDropdownYearsVisibility() {
  var y = document.getElementById("dropDownStates");
  var x = document.getElementById("dropDownYears");
  if (x.style.display === "none") {
	x.style.display = "flex";
	y.style.display = "none";
  } else {
	x.style.display = "none";
  }
}
function submit(startY, endY, s) {
	var form = document.createElement("form");
    	var element1 = document.createElement("input"); 
    	var element2 = document.createElement("input");  
	var element2 = document.createElement("input");  
    	
	form.method = "POST"; 

	element1.value=startY;
	element1.name="startYear";
	form.appendChild(element1);  

	element2.value=endY;
	element2.name="endYear";
	form.appendChild(element2);
	element3.value=s;
	element3.name="state";
	form.appendChild(element3);

    document.body.appendChild(form);

    form.submit();
}
$(document).ready(function () {
toggleDropdownStatesVisibility()	
turnOffAllCollapsibles()
toggleDropdownYearsVisibility()
 }) ;
