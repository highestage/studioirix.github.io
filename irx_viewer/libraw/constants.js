const path = require('path')
const isWin = () => process.platform === 'win32'

const rootDir = __dirname
const librawRoot = path.join(rootDir, 'vendor')
const librawSrc = path.join(librawRoot, 'LibRaw')
const librawBuild = path.join(rootDir, 'build')
const librawLibDir = isWin() ? path.join(librawBuild, 'Release') : path.join(librawBuild, 'libraw')
const librawLocalLib = isWin() ? path.join(librawLibDir, 'libraw.lib') : path.join(librawLibDir, 'libraw.a')

module.exports = {
    rootDir,
    librawRoot,
    librawSrc,
    librawBuild,
    librawLocalLib,
    librawLibDir
}