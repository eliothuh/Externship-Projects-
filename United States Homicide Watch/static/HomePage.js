function toggleDropdownStatesVisibility() {
  var x = document.getElementById("dropDownStates");
  var y = document.getElementById("dropDownYears");
  var z = document.getElementById("nationwide");
  if (x.style.display === "none") {
	x.style.display = "flex";
	y.style.display = "none";
    z.style.display = "none";
  } else {
	x.style.display = "none";
  }
}

function turnOffAllCollapsibles() {
	var y = document.getElementById("dropDownStates");
	var x = document.getElementById("dropDownYears");
    var z = document.getElementById("nationwide");
	x.style.display="none";
	y.style.display="none";
    z.style.display = "none";
}

function toggleDropdownYearsVisibility() {
  var y = document.getElementById("dropDownStates");
  var x = document.getElementById("dropDownYears");
  var z = document.getElementById("nationwide");
  if (x.style.display === "none") {
	x.style.display = "flex";
	y.style.display = "none";
    z.style.display = "none";
  } else {
	x.style.display = "none";
  }
}

function toggleNationwideVisibility() {
  var y = document.getElementById("dropDownStates");
  var x = document.getElementById("dropDownYears");
  var z = document.getElementById("nationwide");
  if (z.style.display === "none") {
	z.style.display = "flex";
	x.style.display = "none";
    y.style.display = "none";
  } else {
	z.style.display = "none";
  }
}
