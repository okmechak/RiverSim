function isBif = BifConditionCheck( bif_type, bif_threshold, a1, a3)
%BifConditionCheck tell us should we biffurcate or not depending on biffurcation
%type and parameters a1 and a2
    isBif = GetBifValue( bif_type, a1, a3) > bif_threshold;
end