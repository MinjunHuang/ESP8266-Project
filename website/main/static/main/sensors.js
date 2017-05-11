var sensorsList;
var currentSensor;

function list() {
	$.get("/api/list/sensors", function(data) {
		sensorsList = data;
		text=''
		$.each(data, function(index, obj) {
			text +=
				'<tr onClick="openModal('+index+');">'+
					'<td>'+obj.mac_addr+'</td>'+
					'<td>'+obj.name+'</td>'+
				'</tr>'
		});
		$("#sensorsTable").html(text)
	});
}

function renameSensor() {
	name=$("#newName").val();
	console.log(name);
	mac=sensorsList[currentSensor].mac_addr;
	$.ajax({
		url: '/api/put/sensor?mac='+mac+"&name="+name,
		type: 'PUT',
		success: function(result) {
			list();
			$("#myModal").modal('hide');
		}
	});
}

function openModal(index) {
	currentSensor = index;
	$("#newName").val(sensorsList[index].name);
	$("#newInterval").val(sensorsList[index].interval);
	$("#myModal").modal();
}

$(document).ready(function() {
	list();
});
