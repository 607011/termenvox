
QT       -= gui

win32:CONFIG(release, debug|release): TARGET = STK
else:win32:CONFIG(debug, debug|release): TARGET = STKd

TEMPLATE = lib
CONFIG += staticlib
VERSION = 4.4.4
DEFINES += __LITTLE_ENDIAN__ __WINDOWS_ASIO__ __WINDOWS_DS__ __WINDOWS_MM__
DEFINES -= UNICODE
INCLUDEPATH += include
LIBS += dsound.lib winmm.lib Wsock32.lib
win32 {
QMAKE_CXXFLAGS += /wd4100 /wd4996 /wd4309 /wd4244
}

SOURCES += Stk.cpp \
    Wurley.cpp \
    Whistle.cpp \
    VoicForm.cpp \
    Voicer.cpp \
    UdpSocket.cpp \
    TwoZero.cpp \
    TwoPole.cpp \
    Twang.cpp \
    TubeBell.cpp \
    Thread.cpp \
    TcpServer.cpp \
    TcpClient.cpp \
    TapDelay.cpp \
    Stk.cpp \
    StifKarp.cpp \
    Sphere.cpp \
    Socket.cpp \
    Skini.cpp \
    Sitar.cpp \
    SingWave.cpp \
    SineWave.cpp \
    Simple.cpp \
    Shakers.cpp \
    Saxofony.cpp \
    Sampler.cpp \
    RtWvOut.cpp \
    RtWvIn.cpp \
    RtMidi.cpp \
    RtAudio.cpp \
    Rhodey.cpp \
    Resonate.cpp \
    PRCRev.cpp \
    PoleZero.cpp \
    Plucked.cpp \
    PitShift.cpp \
    Phonemes.cpp \
    PercFlut.cpp \
    OneZero.cpp \
    OnePole.cpp \
    NRev.cpp \
    Noise.cpp \
    Mutex.cpp \
    Moog.cpp \
    Modulate.cpp \
    ModalBar.cpp \
    Modal.cpp \
    MidiFileIn.cpp \
    Messager.cpp \
    Mesh2D.cpp \
    Mandolin.cpp \
    LentPitShift.cpp \
    JCRev.cpp \
    InetWvOut.cpp \
    InetWvIn.cpp \
    Iir.cpp \
    HevyMetl.cpp \
    Guitar.cpp \
    Granulate.cpp \
    FreeVerb.cpp \
    FormSwep.cpp \
    FMVoices.cpp \
    FM.cpp \
    Flute.cpp \
    Fir.cpp \
    FileWvOut.cpp \
    FileWvIn.cpp \
    FileWrite.cpp \
    FileRead.cpp \
    FileLoop.cpp \
    Envelope.cpp \
    Echo.cpp \
    Drummer.cpp \
    DelayL.cpp \
    DelayA.cpp \
    Delay.cpp \
    Clarinet.cpp \
    Chorus.cpp \
    Brass.cpp \
    Bowed.cpp \
    BlowHole.cpp \
    BlowBotl.cpp \
    BlitSquare.cpp \
    BlitSaw.cpp \
    Blit.cpp \
    BiQuad.cpp \
    BeeThree.cpp \
    BandedWG.cpp \
    Asymp.cpp \
    ADSR.cpp \
    asio/iasiothiscallresolver.cpp \
    asio/asiolist.cpp \
    asio/asiodrivers.cpp \
    asio/asio.cpp

HEADERS += \
    include/soundcard.h \
    include/iasiothiscallresolver.h \
    include/iasiodrv.h \
    include/ginclude.h \
    include/dsound.h \
    include/asiosys.h \
    include/asiolist.h \
    include/asiodrvr.h \
    include/asiodrivers.h \
    include/asio.h \
    include/WvOut.h \
    include/WvIn.h \
    include/Wurley.h \
    include/Whistle.h \
    include/VoicForm.h \
    include/Voicer.h \
    include/Vector3D.h \
    include/UdpSocket.h \
    include/TwoZero.h \
    include/TwoPole.h \
    include/Twang.h \
    include/TubeBell.h \
    include/Thread.h \
    include/TcpServer.h \
    include/TcpClient.h \
    include/TapDelay.h \
    include/Stk.h \
    include/StifKarp.h \
    include/Sphere.h \
    include/Socket.h \
    include/Skini.h \
    include/Sitar.h \
    include/SingWave.h \
    include/SineWave.h \
    include/Simple.h \
    include/Shakers.h \
    include/Saxofony.h \
    include/Sampler.h \
    include/RtWvOut.h \
    include/RtWvIn.h \
    include/RtMidi.h \
    include/RtError.h \
    include/RtAudio.h \
    include/Rhodey.h \
    include/Resonate.h \
    include/ReedTable.h \
    include/PRCRev.h \
    include/PoleZero.h \
    include/Plucked.h \
    include/PitShift.h \
    include/Phonemes.h \
    include/PercFlut.h \
    include/OneZero.h \
    include/OnePole.h \
    include/NRev.h \
    include/Noise.h \
    include/Mutex.h \
    include/Moog.h \
    include/Modulate.h \
    include/ModalBar.h \
    include/Modal.h \
    include/MidiFileIn.h \
    include/Messager.h \
    include/Mesh2D.h \
    include/Mandolin.h \
    include/LentPitShift.h \
    include/JetTable.h \
    include/JCRev.h \
    include/Instrmnt.h \
    include/InetWvOut.h \
    include/InetWvIn.h \
    include/Iir.h \
    include/HevyMetl.h \
    include/Guitar.h \
    include/Granulate.h \
    include/Generator.h \
    include/Function.h \
    include/FreeVerb.h \
    include/FormSwep.h \
    include/FMVoices.h \
    include/FM.h \
    include/Flute.h \
    include/Fir.h \
    include/Filter.h \
    include/FileWvOut.h \
    include/FileWvIn.h \
    include/FileWrite.h \
    include/FileRead.h \
    include/FileLoop.h \
    include/Envelope.h \
    include/Effect.h \
    include/Echo.h \
    include/Drummer.h \
    include/DelayL.h \
    include/DelayA.h \
    include/Delay.h \
    include/Cubic.h \
    include/Clarinet.h \
    include/Chorus.h \
    include/Brass.h \
    include/BowTable.h \
    include/Bowed.h \
    include/BlowHole.h \
    include/BlowBotl.h \
    include/BlitSquare.h \
    include/BlitSaw.h \
    include/Blit.h \
    include/BiQuad.h \
    include/BeeThree.h \
    include/BandedWG.h \
    include/Asymp.h \
    include/ADSR.h \
    SKINI.tbl

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    SKINI.msg
