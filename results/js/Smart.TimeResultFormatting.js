
function getBestTimes(table) {
  const numColumns = table.rows[0].cells.length;
  const best_times = Array(numColumns).fill(999999.99);
  for (let row = 1; row < table.rows.length; row++) {
    const cells = table.rows[row].cells;
    for (let column = 1; column < cells.length; column++) {
      const bestSoFar = best_times[column];
      const divElement = findChildWithClassNames(cells[column], ['search_time', 'search_time_best']);
      const currentValue = parseFloat(divElement.innerText);
      if (currentValue < bestSoFar && currentValue > 0.0) {
         best_times[column] = currentValue;
      }
    }
  }
  return best_times;
}

function heatMapGray(table, cutOffPercent) {
  heatMap(table, 0, 0, cutOffPercent, 100, 95, 30);
}

function heatMapGreen(table, cutOffPercent) {
  heatMap(table, 128, 75, cutOffPercent, 100, 95, 30);
}

function heatMapRed(table, cutOffPercent) {
  heatMap(table, 0, 75, cutOffPercent, 100, 95, 30);
}

function heatMapBlue(table, cutOffPercent) {
  heatMap(table, 240, 75, cutOffPercent, 100, 95, 30);
}

function heatMap(table, hue, saturation, cutOffPercent, cutOffLuminance, worstTimeLuminance, bestTimeLuminance) {
  var luminanceRange = Math.abs(worstTimeLuminance - bestTimeLuminance);
  var best_times = getBestTimes(table);
  for (row = 1; row < table.rows.length; row++) {
    var cells = table.rows[row].cells;
    for (column = 1; column < cells.length; column++) {
      var cell = cells[column];
      var divElement = findChildWithClassNames(cell, ['search_time', 'search_time_best']);
      var currentTime = parseFloat(divElement.innerText);
      var percentOfBest = best_times[column] * 100 / currentTime;
      if (percentOfBest < cutOffPercent) {
        var luminance = cutOffLuminance;
      } else {
        var heatValuePercent = (percentOfBest - cutOffPercent) * (100 / (100 - cutOffPercent));
        var luminanceHeat = luminanceRange * heatValuePercent / 100;
        var luminance = (worstTimeLuminance > bestTimeLuminance? worstTimeLuminance - luminanceHeat : worstTimeLuminance + luminanceHeat);
      }
      divElement.style.backgroundColor = 'hsl(' + hue + ',' + saturation + '%,' + luminance + '%)';
      divElement.style.color = (luminance < 60? '#FFFFFF' : '#000000');
    }
  }
}

function setBestColors(document) {
  var bestTimes = document.getElementsByClassName('search_time_best');
  for(var counter = 0; counter < bestTimes.length; counter++) {
    bestTimes[counter].style.backgroundColor = "#888888";
    bestTimes[counter].style.color = '#FFFFFF';
  }
  var otherTimes = document.getElementsByClassName('search_time');
  for(var counter = 0; counter < otherTimes.length; counter++) {
    otherTimes[counter].style.backgroundColor = "#FFFFFF";
    otherTimes[counter].style.color = "#000000";
  }
}

function showDivs(document, divClass, show) {
  var preProcessingTimes = document.getElementsByClassName(divClass);
  var displayValue = (show? 'block' : 'none');
  for (var counter = 0; counter < preProcessingTimes.length; counter++) {
    preProcessingTimes[counter].style.display = displayValue;
  }
}

function findChildWithClassNames(parent, namesOfClass) {
    var parentChildren = parent.children;
    for (var counter = 0; counter < parentChildren.length; counter++) {
        var child = parentChildren[counter];
        var childIndex = namesOfClass.indexOf(child.className);
        if (childIndex > -1) {
            return child;
        }
        child = findChildWithClassNames(child, namesOfClass);
        if (child != null) {
            return child;
        }
    }
    return null;
}