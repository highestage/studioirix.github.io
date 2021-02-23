const { resolvePath } = require('./utils')
const constants = require('./constants')

const isWin = process.platform === 'win32'

if (isWin) {
    console.log(resolvePath(constants.librawLocalLib))
    return
}

const librawLibDir = constants.librawLibDir

console.log('-Wl,-rpath,' + librawLibDir)

// if (process.platform === 'darwin') {
//     // find X11 lib from XQuartz installation
//     console.log('-L/usr/X11/lib')
// }
// console.log('-lX11')
// console.log('-lpng')