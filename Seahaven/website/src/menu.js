const menu = document.getElementById("menu");
const m_icon = document.getElementById("menuIcon");
const m_volume = document.getElementById("volume");
const m_vSlider = document.getElementById("volumeSlider");
const m_vBall = document.getElementById("volumeBall");
const m_vFill = document.getElementById("volumeFill");
const m_reset = document.getElementById("reset");
const m_wall = document.getElementById("wall");
const m_skin = document.getElementById("skin");
var s_menuOpen = false;
var m_xMin, m_xMax, m_yMin, m_yMax;

function setMenuEvents() {
	document.addEventListener("touchstart", menuHandler);		
    m_reset.addEventListener("touchstart", buttonStart);
    m_reset.addEventListener("touchend", resetEnd);
    m_wall.addEventListener("touchstart", buttonStart);
    m_wall.addEventListener("touchend", wallEnd);
    m_skin.addEventListener("touchstart", buttonStart);
    m_skin.addEventListener("touchend", skinEnd);
    m_vSlider.addEventListener("input", adjustVSlider);
    m_vSlider.addEventListener("change", adjustVolume);
}

function menuHandler(event) {
	(s_menuOpen) ? menuClose(event) : menuOpen(event);
}

function menuOpen(event) {

	m_xMin = (event.touches[0].pageX < 570) ? event.touches[0].pageX : 570;
	m_xMax = m_xMin + 216;
	m_yMin = (event.touches[0].pageY < 300) ? event.touches[0].pageY : 300;
	m_yMax = m_yMin + 166;

	menu.style.left = (m_xMin + 16) + "px";
	menu.style.top = (m_yMin + 16) + "px";

	m_icon.style.visibility = "visible";
	m_volume.style.visibility = "visible";
	setTimeout(function(){m_reset.style.visibility = "visible";},20);
	setTimeout(function(){m_wall.style.visibility = "visible";},40);
	setTimeout(function(){m_skin.style.visibility = "visible";},60);

	s_menuOpen = true;
}

function menuClose(event, hardReset) {

	if (!hardReset &&
		(event.touches[0].pageX > m_xMin && event.touches[0].pageX < m_xMax) && 
	    (event.touches[0].pageY > m_yMin && event.touches[0].pageY < m_yMax)) {
		return;
	} 

	m_icon.style.visibility = "hidden";
	m_volume.style.visibility = "hidden";
	m_reset.style.visibility = "hidden";
	m_wall.style.visibility = "hidden";
	m_skin.style.visibility = "hidden";
	s_menuOpen = false;
}

function buttonStart(event) {
	event.srcElement.style.background = "#82B1FF";
	event.srcElement.style.color = "black";
}

function buttonEnd(element) {
	element.style.background = "#448AFF";
	element.style.color = "white";
	menuClose(0, true);
}

function resetEnd(event) {
	buttonEnd(m_reset);	
	// location.reload();
}

function wallEnd(event) {
	buttonEnd(m_wall);
	changeWall();
}

function skinEnd(event) {
	buttonEnd(m_skin);
}

function adjustVSlider(event) {
	m_vBall.style.left = (event.srcElement.value * 1.48) + "px";
	m_vFill.style.width = (event.srcElement.value * 0.8) + "%";
}

function adjustVolume(event) {	
	wsVolume(event.srcElement.value);
}