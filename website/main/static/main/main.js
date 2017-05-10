var sensorsList;

function listSensors() {
	$.get('/api/list/sensors', function(data) {
		sensorsList = data;
		text='<option value="">Any</option>';
		$.each(data, function(index, obj) {
			text+='<option value='+obj.mac_addr+'>'+obj.name+'</option>'
		});
		$("#sensorSelect").html(text)
	});
}

function list() {
	mac = $("#sensorSelect option:selected").val();
	var beginDate = $("#beginDate").val();
	var endDate = $("#endDate").val();
	$.get("/api/list/measurements?beginDate="+beginDate+"&endDate="+endDate+"&mac="+mac, function(data) {
		text = '';
		$.each(data, function(index, obj) {
			date = String(obj.created).match(/^.*T/)[0].slice(0, -1);
			time = String(obj.created).match(/T.*\./)[0].slice(1, -1);
			text +=
				'<tr>'+
					'<td>'+date+'</td>'+
					'<td>'+time+'</td>'+
					'<td>'+obj.sensor.name+'</td>'+
					'<td>'+obj.temperature+'&#8451;</td>'+
					'<td>'+obj.humidity+'%</td>'+
				'</tr>'
		});
		$("#dataTable").html(text);
	});
}

function refresh() {
	list();
}

$(document).ready(function() {
	listSensors();
	list();
});
