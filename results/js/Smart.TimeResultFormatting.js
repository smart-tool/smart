
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
  const luminanceRange = Math.abs(worstTimeLuminance - bestTimeLuminance);
  const best_times = getBestTimes(table);
  for (let row = 1; row < table.rows.length; row++) {
    const cells = table.rows[row].cells;
    for (let column = 1; column < cells.length; column++) {
      const cell = cells[column];
      const divElement = findChildWithClassNames(cell, ['search_time', 'search_time_best']);
      const currentTime = parseFloat(divElement.innerText);
      const percentOfBest = best_times[column] * 100 / currentTime;
      let luminance = cutOffLuminance;
      if (percentOfBest >= cutOffPercent) {
        const heatValuePercent = (percentOfBest - cutOffPercent) * (100 / (100 - cutOffPercent));
        const luminanceHeat = luminanceRange * heatValuePercent / 100;
        luminance = (worstTimeLuminance > bestTimeLuminance? worstTimeLuminance - luminanceHeat : worstTimeLuminance + luminanceHeat);
      }
      divElement.style.backgroundColor = 'hsl(' + hue + ',' + saturation + '%,' + luminance + '%)';
      divElement.style.color = (luminance < 60? '#FFFFFF' : '#000000');
    }
  }
}

function setBestColors(document) {
  const bestTimes = document.getElementsByClassName('search_time_best');
  for(let counter = 0; counter < bestTimes.length; counter++) {
    bestTimes[counter].style.backgroundColor = "#888888";
    bestTimes[counter].style.color = '#FFFFFF';
  }
  const otherTimes = document.getElementsByClassName('search_time');
  for(let counter = 0; counter < otherTimes.length; counter++) {
    otherTimes[counter].style.backgroundColor = "#FFFFFF";
    otherTimes[counter].style.color = "#000000";
  }
}

function showDivs(document, divClass, show) {
  const preProcessingTimes = document.getElementsByClassName(divClass);
  const displayValue = show ? 'block' : 'none';
  for (const counter in preProcessingTimes) {
    preProcessingTimes[counter].style.display = displayValue;
  }
}

function showDivsByName(document, divName, show) {
    const divs = document.getElementsByName(divName);
    const displayValue = show ? 'block' : 'none';
    for (const counter in divs) {
        divs[counter].style.display = displayValue;
    }
}

function findChildWithClassNames(parent, namesOfClass) {
    const parentChildren = parent.children;
    for (let counter = 0; counter < parentChildren.length; counter++) {
        const child = parentChildren[counter];
        const childIndex = namesOfClass.indexOf(child.className);
        if (childIndex > -1) {
            return child;
        }
        const subchild = findChildWithClassNames(child, namesOfClass);
        if (subchild != null) {
            return subchild;
        }
    }
    return null;
}
