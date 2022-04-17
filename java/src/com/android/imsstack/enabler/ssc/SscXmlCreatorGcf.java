package com.android.imsstack.enabler.ssc;

import com.android.imsstack.enabler.ssc.data.CfServiceData;
import com.android.imsstack.enabler.ssc.data.SscRuleData;
import com.android.imsstack.enabler.ssc.data.SscRuleElement;
import com.android.imsstack.enabler.ssc.data.SscServiceData;
import com.android.imsstack.util.ImsLog;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

//// XML DOC is created to use for PUT REQUEST ONLY

public class SscXmlCreatorGcf extends SscXmlCreator {
    // Constants--------------------------------------------------
    // Variables--------------------------------------------------

    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    public SscXmlCreatorGcf() {
        super();
        ImsLog.d("");

        if (mXMLCreatorTable != null) {
            mXMLCreatorTable.put(ESsType.CF, new XmlCreatorCfGcf());
        }
    }

    //@Override
    protected Document createXMLDocForState(int slotId, String element, int state) {
        Document doc = null;
        try {
            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
            doc = docBuilder.newDocument();

            doc.setXmlStandalone(true);

            if (state != -1) {
                //Element enable = doc.createElement(SscXmlFormat.getXMLSS(slotId) + element);
                Element enable = doc.createElement(element);

                enable.setAttribute(SscXmlFormat.ACTIVE, ((state == 1) ? "true" : "false"));
                doc.appendChild(enable);
            }
        }
        catch (ParserConfigurationException pce) {
            ImsLog.d(pce.toString());
            pce.printStackTrace();
            return null;
        }

        return doc;
    }

    // Private/Protected methods ---------------------------------
    // -----------------------------------------------------------
    protected class XmlCreatorCfGcf implements IXmlCreator {
        @Override
        public Element createXMLElement(Document doc, SscServiceData data) {
            return null;
        }
/*
        public Document createXMLDoc(SscServiceData inputData, boolean isVideo) {
            ImsLog.d("");

            // cast to CfServiceData
            if (!(inputData instanceof CfServiceData)) {
                ImsLog.e("data type is not valid");
                return null;
            }

            CfServiceData data = (CfServiceData)inputData;
            // If NoReplyTimer is set, then it cann't update CF rule item at once.
            // (based on IR 92 Requirement)
            if (data.getRuleSet() == null) {
                if (!SscConfig.CF.isNeedTimerInNoAnswer(data.getSlotId())) {
                    if (data.getNoReplyTimer() > 0) {
                        return createXMLDocNoReplyTimer(data.getSlotId(), data.getNoReplyTimer());
                    }
                    else {
                        ImsLog.e("NoReplyTimer is invalid - " + data.getNoReplyTimer() + " <= 0");
                    }
                }
            }
            else {
                switch (data.getState()) {
                    // if action is registration or erasure, create xml with conditions & actions
                    case SscConstant.ACTION_REGISTRATION :
                    case SscConstant.ACTION_ERASURE :
                    case SscConstant.ACTION_ACTIVATION :
                    case SscConstant.ACTION_DEACTIVATION :
                        return createXMLDocCF(data, isVideo);
                    default :
                        ImsLog.e("Invalid Action(" + data.getState() + ")");
                        break;
                }
            }

            return null;
        }

        protected Document createXMLDocNoReplyTimer(int slotId, int noReplyTimer) {
            ImsLog.d("NoReplyTimer/SlotId : " + noReplyTimer + "/" + slotId);

            Document doc = null;

            try {
                DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
                DocumentBuilder docBuilder = docFactory.newDocumentBuilder();

                doc = docBuilder.newDocument();
                doc.setXmlStandalone(true);

                Element elemNoReplyTimer = doc.createElement(
                        SscXmlFormat.getNoReplyTimer(slotId));
                elemNoReplyTimer.setTextContent(Integer.toString(noReplyTimer));
                doc.appendChild(elemNoReplyTimer);
            }
            catch (ParserConfigurationException pce) {
                ImsLog.d(pce.toString());
                pce.printStackTrace();
            }

            return doc;
        }

        protected Document createXMLDocCFCondition(CfServiceData inputData, boolean isVideo) {

            Document doc = null;
            Element ruleSet = null;
            try {
                DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
                DocumentBuilder docBuilder = docFactory.newDocumentBuilder();

                doc = docBuilder.newDocument();
                doc.setXmlStandalone(true);

                if (inputData.getRuleSet().size() > 1) {
                    ruleSet = doc.createElement(SscXmlFormat.getRuleSet(inputData.getSlotId()));
                    doc.appendChild(ruleSet);
                }

                // MUST BE One RuleSet!!
                for (SscRuleData ruleData : inputData.getRuleSet()) {
                    Element rule = doc.createElement(SscXmlFormat.getRule(inputData.getSlotId()));
                    rule.setAttribute(SscXmlFormat.ID, ruleData.getRuleId());
                    ImsLog.d("rule id = " + ruleData.getRuleId());

                    // condition
                    Element conditions = doc.createElement(
                            SscXmlFormat.getConditions(inputData.getSlotId()));
                    rule.appendChild(conditions);

                    for (SscRuleElement element : ruleData.getConditionList()) {
                        Element condElement = doc.createElement(
                                SscXmlFormat.getXMLSS(inputData.getSlotId()) + element.getKey());
                        ImsLog.d("condElement : " + element.getKey());

                        // Media case
                        if (SscXmlFormat.MEDIA.equals(element.getKey())) {
                            condElement.setTextContent(element.getValue());
                        }
                        // Identity case
                        else if (SscXmlFormat.IDENTITY.equals(element.getKey())) {
                            Element one = doc.createElement(
                                    SscXmlFormat.getOne(inputData.getSlotId()));
                            one.setTextContent(element.getValue());
                            condElement.appendChild(one);
                        }
                        conditions.appendChild(condElement);
                    }

                    if (ruleSet != null) {
                        ruleSet.appendChild(rule);
                    }
                    else {
                        doc.appendChild(conditions);
                    }
                }
            }
            catch (ParserConfigurationException pce) {
                ImsLog.d(pce.toString());
                pce.printStackTrace();
            }

            return doc;
        }

        protected Document createXMLDocCF(CfServiceData inputData, boolean isVideo) {

            Document doc = null;
            Element ruleSet = null;
            try {
                DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
                DocumentBuilder docBuilder = docFactory.newDocumentBuilder();

                doc = docBuilder.newDocument();
                doc.setXmlStandalone(true);

                Element communicationdiversion = doc.createElement(
                      SscXmlFormat.getXMLSS(inputData.getSlotId()) + "communication-diversion");

                switch (inputData.getState()) {
                    case SscConstant.ACTION_REGISTRATION :
                    case SscConstant.ACTION_ACTIVATION :
                        communicationdiversion.setAttribute("active", "true");
                        break;
                    case SscConstant.ACTION_DEACTIVATION :
                        communicationdiversion.setAttribute("active", "false");
                        doc.appendChild(communicationdiversion);
                        return doc;
                    default :
                        break;
                }

//                if (SscXmlFormat.getIsCFRulesetPresent(inputData.getSlotId()) == false ) {
//                    ruleSet = doc.createElement(SscXmlFormat.getRuleSet(inputData.getSlotId()));
//                    doc.appendChild(ruleSet);
//                }
//                else {
//                    // rule set present so leave it to default one.
//                }
//                if (null == ruleSet && inputData.getRuleSet().size() > 1) {
//                    ruleSet = doc.createElement(SscXmlFormat.getRuleSet(inputData.getSlotId()));
//                    doc.appendChild(ruleSet);
//                }


                // MUST BE One RuleSet!!
                for (SscRuleData ruleData : inputData.getRuleSet()) {
                    ruleSet = doc.createElement(SscXmlFormat.getRuleSet(inputData.getSlotId()));
                    communicationdiversion.appendChild(ruleSet);

                    Element rule = doc.createElement(SscXmlFormat.getRule(inputData.getSlotId()));
                    rule.setAttribute(SscXmlFormat.ID, ruleData.getRuleId());
                    ruleSet.appendChild(rule);

                    // condition
                    Element conditions = doc.createElement(
                            SscXmlFormat.getConditions(inputData.getSlotId()));
                    rule.appendChild(conditions);

                    for (SscRuleElement element : ruleData.getConditionList()) {
                        //Element condElement = doc.createElement(
                                //SscXmlFormat.getXMLSS() + element.getKey());
                        Element condElement = doc.createElement(element.getKey());
                        ImsLog.d("condElement : " + element.getKey());

                        // Media case
                        if (SscXmlFormat.MEDIA.equals(element.getKey())) {
                            condElement.setTextContent(element.getValue());
                        }
                        // Identity case
                        else if (SscXmlFormat.IDENTITY.equals(element.getKey())) {
                            Element one = doc.createElement(
                                    SscXmlFormat.getOne(inputData.getSlotId()));
                            one.setTextContent(element.getValue());
                            condElement.appendChild(one);
                        }
                        conditions.appendChild(condElement);
                    }

                    // action
                    Element actions = doc.createElement(
                            SscXmlFormat.getActions(inputData.getSlotId()));

                    rule.appendChild(actions);
                    for (SscRuleElement element : ruleData.getActionList()) {
                        //Element actionElement = doc.createElement(
                              //  SscXmlFormat.getXMLSS() + element.getKey());
                        Element actionElement = doc.createElement(element.getKey());
                        ImsLog.d("actionElement : " + element.getKey());

                        // forward-to
                        if (SscXmlFormat.FORWARD_TO.equals(element.getKey())) {
                            //Element target = doc.createElement(SscXmlFormat.getTarget());
                            Element target = doc.createElement("target");
                            String targetNumber = makeValueInTargetTo(
                                    inputData.getSlotId(), element.getValue());
                            ImsLog.d("targetNumber : " + targetNumber);
                            target.setTextContent(targetNumber);
                            actionElement.appendChild(target);
                        }
                        else {
                            actionElement.setTextContent((element.getValue()));
                        }
                        actions.appendChild(actionElement);

                        if (SscConfig.CF.isNeedTimerInNoAnswer(inputData.getSlotId())
                                && inputData.getReqCondition() == SscConstant.CONDITION_CFNR) {
                            // send NoReplyTimer element in 1st PUT
                            if (inputData.getNoReplyTimer() > 0) {
                                int noReplyTimer = inputData.getNoReplyTimer();
                                Element elemNoReplyTimer = doc.createElement(
                                        SscXmlFormat.getNoReplyTimer(inputData.getSlotId()));
                                elemNoReplyTimer.setTextContent(Integer.toString(noReplyTimer));
                                actions.appendChild(elemNoReplyTimer);
                            }
                        }
                    }

                    doc.appendChild(communicationdiversion);
                }
            }
            catch (ParserConfigurationException pce) {
                ImsLog.d(pce.toString());
                pce.printStackTrace();
            }

            return doc;
        }

        protected String makeValueInTargetTo(int slotId, String number) {
            if (number == null || number.isEmpty()) {
                ImsLog.e("Number is invalid !!!");
                return null;
            }

            String domain = SscConfig.Subscriber.getDomain(slotId);

            String addressing = null;
            final String format = SscConfig.MMTel.getTarget(slotId);
            ImsLog.d("number : " + number + ", format : " + format + ", domain : " + domain);
            // +4477009900123 -> sip:+4477009900123@example.com;user=phone
            // 004477009900123
            // -> sip:004477009900123;phone-context=example.com@example.com;user=phone
            if ("sip".equalsIgnoreCase(format)) {
                addressing = "sip:" + number;
                // local numbering
                if (!number.startsWith("+")) {
                    addressing += ";phone-context=" + domain;
                }
                addressing += "@" + domain + ";user=phone";
            }
            // +4477009900123 -> tel:+4477009900123
            // 004477009900123 -> tel:004477009900123;phone-context=exampl.com
            else if ("tel".equalsIgnoreCase(format)) {
                addressing = "tel:" + number;
                // local numbering
                if (!number.startsWith("+")) {
                    addressing += ";phone-context=" + domain;
                }
            }
            else {
                addressing = number;
            }

            return addressing;
        }
*/
    }
}
