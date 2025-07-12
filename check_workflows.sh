#!/bin/bash

# Simple script to check if GitHub workflows are running
# by monitoring git commits and remote refs

echo "🔍 Checking workflow status for JuncTek_BatteryMonitor..."
echo "Repository: https://github.com/pljakobs/JuncTek_Batterymonitor"
echo "Actions URL: https://github.com/pljakobs/JuncTek_Batterymonitor/actions"
echo ""

echo "📋 Recent commits on copilot/fix-2:"
git log --oneline -5 origin/copilot/fix-2

echo ""
echo "🚀 Workflow triggers should now activate on pushes to copilot/fix-* branches"
echo "💡 Check the Actions tab in your GitHub repository to see running workflows"
echo ""
echo "⚡ If workflows are still not visible, check:"
echo "  1. Repository settings → Actions → General → Allow all actions"
echo "  2. Repository has .github/workflows/ directory with valid YAML files"
echo "  3. Branch protection rules don't block workflow execution"
