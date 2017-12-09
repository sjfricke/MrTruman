const menu = document.getElementById("menu");
const m_icon = document.getElementById("menuIcon");
const m_volume = document.getElementById("volume");
const m_vSlider = document.getElementById("volumeSlider");
const m_vBall = document.getElementById("volumeBall");
const m_vFill = document.getElementById("volumeFill");
const m_wall = document.getElementById("wall");
const m_power = document.getElementById("power");
const m_halt = document.getElementById("halt");
const m_reboot = document.getElementById("reboot");
const m_filter = document.getElementById("filter");
const m_xMax_show = 570;
const m_yMax_show = 358;
var s_menuOpen = false;
var s_powerOpen = false;
var powerTimeout;
var m_xMin, m_xMax, m_yMin, m_yMax;
var filterStatus = 0;
var invaildSlider = false;

function setMenuEvents() {
    document.addEventListener("touchstart", menuHandler);		
    m_wall.addEventListener("touchstart", buttonStart);
    m_wall.addEventListener("touchend", wallEnd);
    m_filter.addEventListener("touchstart", buttonStart);
    m_filter.addEventListener("touchstart", powerTimer);
    m_filter.addEventListener("touchend", filterEnd);
    m_vSlider.addEventListener("touchstart", checkValidSlider);
    m_vSlider.addEventListener("touchend", uncheckValidSlider);
    m_vSlider.addEventListener("input", adjustVSlider);
}

function menuHandler(event) {
	if (s_powerOpen) {
		m_power.style.visibility = "hidden";
		s_powerOpen = false;
	} else {
		(s_menuOpen) ? menuClose(event) : menuOpen(event);
	}	
}

function menuOpen(event) {
	m_xMin = (event.touches[0].pageX < m_xMax_show) ? event.touches[0].pageX : m_xMax_show;
	m_xMax = m_xMin + 222;
    m_yMin = (event.touches[0].pageY < m_yMax_show) ? event.touches[0].pageY : m_yMax_show;
	m_yMax = m_yMin + 120;

	menu.style.left = (m_xMin + 24) + "px";
	menu.style.top = (m_yMin - 16) + "px";

	m_icon.style.visibility = "visible";
	m_volume.style.visibility = "visible";
	setTimeout(function(){m_wall.style.visibility = "visible";},30);
    setTimeout(function(){m_filter.style.visibility = "visible";},60);

	s_menuOpen = true;
}

function menuClose(event, hardReset) {

	if (!hardReset &&
		(event.touches[0].pageX > m_xMin + 20 && event.touches[0].pageX < m_xMax + 10) && 
	    (event.touches[0].pageY > m_yMin - 30 && event.touches[0].pageY < m_yMax)) {
		return;
	} 

	m_icon.style.visibility = "hidden";
	m_volume.style.visibility = "hidden";
	m_wall.style.visibility = "hidden";
    m_filter.style.visibility = "hidden";
	s_menuOpen = false;
}

function buttonStart(event) {
	if (invaildSlider) {
		m_wall.style.background = "#82B1FF";
		m_wall.style.color = "black";
	} else {
		event.srcElement.style.background = "#82B1FF";
		event.srcElement.style.color = "black";
	}
}

function buttonEnd(element) {
	clearTimeout(powerTimeout);

	if (invaildSlider) {
		m_wall.style.background = "#448AFF";
		m_wall.style.color = "white";
	} else {
		element.style.background = "#448AFF";
		element.style.color = "white";
	}	
	menuClose(0, true);
}


function wallEnd(event) {
	buttonEnd(m_wall);
	changeWall();
}

function filterEnd(event) {
	buttonEnd(m_filter);
	if (s_powerOpen) { return; }

	filterStatus++;
	if (filterStatus > 2) {filterStatus = 0;}

	if (filterStatus == 0) {
		renderer.app.stage.filters = [];		
		renderer.app.renderer.clearBeforeRender = false; // set true from ascii
	} else if (filterStatus == 1) {
		renderer.app.stage.filters = [ embossFilter ];
	} else {
		renderer.app.stage.filters = [ asciiFilter ]; 
		renderer.app.renderer.clearBeforeRender = true;
	}
}

function checkValidSlider(event) {
	if (event.touches[0].pageY > m_yMin + 28) {
		invaildSlider = true;
		buttonStart();
	}
}
function uncheckValidSlider(event) {
	if (invaildSlider) {
		wallEnd();
	}
	invaildSlider = false;
}
function adjustVSlider(event) {
	if (invaildSlider) { return; }
    wsVolume(event.srcElement.value);
	m_vBall.style.left = (event.srcElement.value * 1.48) + "px";
	m_vFill.style.width = (event.srcElement.value * 0.8) + "%";
}

function powerTimer(event) {
	powerTimeout = setTimeout(function(){
		m_power.style.visibility = "visible";
		s_powerOpen = true;
	}, 4000);
}

function changeWall() {
    let wall = renderer.getElemByID("wall");

    currentWall++;
    if (currentWall > 4) { currentWall = 0 }

    wall.texture = wallTextures[currentWall];
}

//window.addEventListener("touchstart", menuDebug, false);

function menuDebug(event) {
	log("menu", "", event.touches[0].pageX, event.touches[0].pageY, event.touches[0].target);
	log("menu", "", event);
}