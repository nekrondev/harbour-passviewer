.pragma library

function interpretColor(color) {
    if (color.substring(0,3) !== 'rgb')
        return color;
    var components = color.match(/\d+/g);
    var code = '#';
    for (var cur = 0; cur < 3; cur++) {
        var value = parseInt(components[cur]);
        if (isNaN(value) || value < 0)
            value = 0;
        if (value > 0xff)
            value = 0xff;
        if (value < 0x10)
            code += '0';
        code += value.toString(16);
    }
    return code;
}

function barcodeSize(origWidth, origHeight, maxWidth) {
    var width = 0;
    var height = 0;
    var scale = 0;
    while (width + 20 <= maxWidth && height < 120) {
        scale++;
        width = origWidth * scale;
        height = origHeight * scale;
    }
    if (width + 20 > maxWidth && scale > 1) {
        scale--;
        width = origWidth * scale;
        height = origHeight * scale;
    }
    return [width, height];
}
