#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=DisplayMain.c DisplayConfiguration.c DisplayMenu.c DisplayRTCC.c Delays.c Communications.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/DisplayMain.o ${OBJECTDIR}/DisplayConfiguration.o ${OBJECTDIR}/DisplayMenu.o ${OBJECTDIR}/DisplayRTCC.o ${OBJECTDIR}/Delays.o ${OBJECTDIR}/Communications.o
POSSIBLE_DEPFILES=${OBJECTDIR}/DisplayMain.o.d ${OBJECTDIR}/DisplayConfiguration.o.d ${OBJECTDIR}/DisplayMenu.o.d ${OBJECTDIR}/DisplayRTCC.o.d ${OBJECTDIR}/Delays.o.d ${OBJECTDIR}/Communications.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/DisplayMain.o ${OBJECTDIR}/DisplayConfiguration.o ${OBJECTDIR}/DisplayMenu.o ${OBJECTDIR}/DisplayRTCC.o ${OBJECTDIR}/Delays.o ${OBJECTDIR}/Communications.o

# Source Files
SOURCEFILES=DisplayMain.c DisplayConfiguration.c DisplayMenu.c DisplayRTCC.c Delays.c Communications.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FV32KA301
MP_LINKER_FILE_OPTION=,--script=p24FV32KA301.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/DisplayMain.o: DisplayMain.c  .generated_files/flags/default/2c76f49fceafac94f74fb07532330ec809a2c101 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayMain.o.d 
	@${RM} ${OBJECTDIR}/DisplayMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayMain.c  -o ${OBJECTDIR}/DisplayMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayMain.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DisplayConfiguration.o: DisplayConfiguration.c  .generated_files/flags/default/8a1f7b2417f1f141efda243f1dadc4c919f03bb4 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayConfiguration.o.d 
	@${RM} ${OBJECTDIR}/DisplayConfiguration.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayConfiguration.c  -o ${OBJECTDIR}/DisplayConfiguration.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayConfiguration.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DisplayMenu.o: DisplayMenu.c  .generated_files/flags/default/34b005bce218f587dab7bbfe73144bb239dff6ed .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayMenu.o.d 
	@${RM} ${OBJECTDIR}/DisplayMenu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayMenu.c  -o ${OBJECTDIR}/DisplayMenu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayMenu.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DisplayRTCC.o: DisplayRTCC.c  .generated_files/flags/default/1435efdbad23eb50f411abd28690f5eb7a5fa96b .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayRTCC.o.d 
	@${RM} ${OBJECTDIR}/DisplayRTCC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayRTCC.c  -o ${OBJECTDIR}/DisplayRTCC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayRTCC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Delays.o: Delays.c  .generated_files/flags/default/9f9f4763de47a502c293e8a7141649f643ef69cc .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Delays.o.d 
	@${RM} ${OBJECTDIR}/Delays.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Delays.c  -o ${OBJECTDIR}/Delays.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Delays.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Communications.o: Communications.c  .generated_files/flags/default/8749625914677004636b863ca45ed8696be4a392 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Communications.o.d 
	@${RM} ${OBJECTDIR}/Communications.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Communications.c  -o ${OBJECTDIR}/Communications.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Communications.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/DisplayMain.o: DisplayMain.c  .generated_files/flags/default/b2968591dd17935fd2b75a183ec51d632ab97d0 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayMain.o.d 
	@${RM} ${OBJECTDIR}/DisplayMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayMain.c  -o ${OBJECTDIR}/DisplayMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayMain.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DisplayConfiguration.o: DisplayConfiguration.c  .generated_files/flags/default/6f228706265a6af223bb701b4700da4c39ccf6e7 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayConfiguration.o.d 
	@${RM} ${OBJECTDIR}/DisplayConfiguration.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayConfiguration.c  -o ${OBJECTDIR}/DisplayConfiguration.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayConfiguration.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DisplayMenu.o: DisplayMenu.c  .generated_files/flags/default/d8a20f36cced4cabdfcb8ec902dbf39e5fb89e59 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayMenu.o.d 
	@${RM} ${OBJECTDIR}/DisplayMenu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayMenu.c  -o ${OBJECTDIR}/DisplayMenu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayMenu.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/DisplayRTCC.o: DisplayRTCC.c  .generated_files/flags/default/8a23783079e2f344386154b01846bdcfe249cf86 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DisplayRTCC.o.d 
	@${RM} ${OBJECTDIR}/DisplayRTCC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DisplayRTCC.c  -o ${OBJECTDIR}/DisplayRTCC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DisplayRTCC.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Delays.o: Delays.c  .generated_files/flags/default/691b112bead4bb0bc1237acdd7b67fbb1e9da450 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Delays.o.d 
	@${RM} ${OBJECTDIR}/Delays.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Delays.c  -o ${OBJECTDIR}/Delays.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Delays.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Communications.o: Communications.c  .generated_files/flags/default/9f71be10c0d3d77fff2b52a2aff2cc68f0bbe6a4 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Communications.o.d 
	@${RM} ${OBJECTDIR}/Communications.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Communications.c  -o ${OBJECTDIR}/Communications.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Communications.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_UserInterface.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
