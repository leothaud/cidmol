
import core;

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  // if (argc == 0) {
  //   core::serr << "Argument expected.\n";
  //   return 1;
  // }
  // //
  // _ZN4coreW4core13LogicalResultINS_S0_9UniquePtrINS_S0_15DemangledSymbolEEEEC2IPNS_S0_11ConstSymbolEQ15isConstructibleIT_TL0__EEEOS9_
  auto demangled = core::demangle(
      "_ZN4coreW4core15DemangledSymbolCI2NS_S0_7VariantIJNS_S0_"
      "14OperatorSymbolENS_S0_13VirtualSymbolENS_S0_17ConstructorSymbolENS_S0_"
      "16DestructorSymbolENS_S0_9StdSymbolENS_S0_10BaseSymbolENS_S0_"
      "13BuiltinSymbolENS_S0_13RequireSymbolENS_S0_14TemplateSymbolENS_S0_"
      "22TemplateArgumentSymbolENS_S0_11ChainSymbolENS_S0_"
      "16ModuleBaseSymbolENS_S0_12ModuleSymbolENS_S0_6SymbolENS_S0_"
      "9PtrSymbolENS_S0_10RvalSymbolENS_S0_10LvalSymbolENS_S0_"
      "13FunTypeSymbolENS_S0_11ConstSymbolENS_S0_14VolatileSymbolENS_S0_"
      "12LambdaSymbolENS_S0_13BinExprSymbolEEEEIS6_EEOT_NS_S0_"
      "12EnableIfBaseIXoo6isSameISP_S3_Eoo6isSameISP_S4_Eoo6isSameISP_S5_"
      "Eoo6isSameISP_S6_Eoo6isSameISP_S7_Eoo6isSameISP_S8_Eoo6isSameISP_S9_"
      "Eoo6isSameISP_SA_Eoo6isSameISP_SB_Eoo6isSameISP_SC_Eoo6isSameISP_SD_"
      "Eoo6isSameISP_SE_Eoo6isSameISP_SF_Eoo6isSameISP_SG_Eoo6isSameISP_SH_"
      "Eoo6isSameISP_SI_Eoo6isSameISP_SJ_Eoo6isSameISP_SK_Eoo6isSameISP_SL_"
      "Eoo6isSameISP_SM_Eoo6isSameISP_SN_E6isSameISP_SO_EEiE4typeE"
      "EEDcSR_");
  if (demangled.failed()) {
    core::serr << demangled.getMessage() << "\n";
    return 1;
  }
  core::sout << *demangled << "\n";
  return 0;
}